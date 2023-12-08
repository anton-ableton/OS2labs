// Compile: gcc main.c -o main -lpthread
// Test: curl -x http://localhost:80 http://google.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 1024

void *handle_client(void *arg) {
  int client_socket = *(int *)arg;
  char buf[BUF_SIZE];

  // Read the HTTP request from the client
  read(client_socket, buf, BUF_SIZE);

  // Parse the HTTP request to determine the server to which the request should be forwarded
  // This is a very basic example and doesn't handle all possible cases
  char *server_host = strtok(buf, " ");
  server_host = strtok(NULL, " ");

  // Connect to the server
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(80);
  inet_pton(AF_INET, server_host, &server_addr.sin_addr);
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Forward the HTTP request to the server
  write(server_socket, buf, strlen(buf));

  // Read the HTTP response from the server
  memset(buf, 0, BUF_SIZE);
  read(server_socket, buf, BUF_SIZE);

  // Forward the HTTP response back to the client
  write(client_socket, buf, strlen(buf));

  // Close the sockets
  close(client_socket);
  close(server_socket);

  return NULL;
}

int main() {
  // Create a socket to listen for incoming connections
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // Bind the socket to a port
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(80);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Start listening for incoming connections
  listen(server_socket, 5);

  printf("Proxy is working on port %d\n", server_addr.sin_port);

  // Accept incoming connections in a loop
  while (1) {
    int client_socket = accept(server_socket, NULL, NULL);

    // Create a new thread to handle the communication
    pthread_t thread;
    pthread_create(&thread, NULL, handle_client, &client_socket);
  }

  return 0;
}
