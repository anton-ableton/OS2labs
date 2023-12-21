#define _GNU_SOURCE

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80
#define BUFFER_SIZE 4096

typedef struct
{
    int client_socket;
    char *request;
} context;

int create_server_socket()
{
    struct sockaddr_in server_addr;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Error while creating server socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error while binding server socket");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, SOMAXCONN) == -1)
    {
        perror("Error while listening on server socket");
        close(server_socket);
        return -1;
    }

    return server_socket;
}

int read_request(int client_socket, char *request)
{
    ssize_t bytes_read = recv(client_socket, request, BUFFER_SIZE, 0);
    if (bytes_read == -1)
    {
        perror("Error while reading request from client");
        return -1;
    }
    else if (bytes_read == 0)
    {
        fprintf(stderr, "Client disconnected\n");
        return -1;
    }

    request[bytes_read] = '\0';
    printf("Received from client: %s\n", request);
    return 0;
}

int connect_to_remote(char *host)
{

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host, "80", &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "Error getting address info: %s\n", gai_strerror(status));
        return -1;
    }

    int dest_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (dest_socket == -1)
    {
        perror("Error while creating remote server socket");
        freeaddrinfo(res);
        return -1;
    }

    if (connect(dest_socket, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("Error while connecting to remote server");
        close(dest_socket);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return dest_socket;
}

void *client_handler(void *arg)
{

    context *ctx = (context *)arg;
    int client_socket = ctx->client_socket;
    char *request0 = ctx->request;
    char request[BUFFER_SIZE];
    strcpy(request, request0);

    unsigned char host[1000];
    unsigned char *host_result = memcpy(host, strstr((char *)request, "Host:") + 6, sizeof(host));
    for (int i = 0; i < sizeof(host); i++)
    {
        if (host_result[i] == ':' || host_result[i] == '\n' || host_result[i] == '\r' || host_result[i] == ' ')
        {
            host_result[i] = '\0';
            break;
        }
    }
    host[host_result - host - 1] = '\0';

    int dest_socket = connect_to_remote((char *)host_result);
    if (dest_socket == -1)
    {
        close(client_socket);
        return NULL;
    }

    ssize_t bytes_sent = write(dest_socket, request, strlen(request));
    if (bytes_sent == -1)
    {
        close(client_socket);
        close(dest_socket);
        return NULL;
    }

    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    ssize_t bytes_read;
    while ((bytes_read = read(dest_socket, buffer, BUFFER_SIZE)) > 0)
    {
        printf("Received from remote server: %s\n", buffer);
        bytes_sent = write(client_socket, buffer, bytes_read);
        if (bytes_sent == -1)
        {
            close(client_socket);
            close(dest_socket);
            free(buffer);
            return NULL;
        }
        printf("Sent to client: %s\n", buffer);
    }

    close(client_socket);
    close(dest_socket);
    free(buffer);
    free(request0);

    return NULL;
}

int main()
{
    int port;
    printf("Enter port: ");
    scanf("%d", &port);

    printf("HTTP-proxy started on port %d\n", port);
    int server_socket = create_server_socket();
    if (server_socket == -1)
    {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_socket == -1)
        {
            perror("Error accepting client connection");
            continue;
        }

        char *request = calloc(BUFFER_SIZE, sizeof(char));
        int err = read_request(client_socket, request);
        if (err == -1)
        {
            perror("Error reading client request");
            free(request);
            close(client_socket);
            continue;
        }

        context ctx = {client_socket, request};
        pthread_t handler_thread;
        err = pthread_create(&handler_thread, NULL, &client_handler, &ctx);
        if (err != 0)
        {
            perror("Error creating handler thread");
            free(request);
            close(client_socket);
            continue;
        }

        pthread_detach(handler_thread);
    }

    close(server_socket);
    return 0;
}
