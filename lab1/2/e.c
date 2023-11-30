//gcc e.c -o e -pthread

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *thread_function(void *arg) {
  char *message = (char *)malloc(sizeof(char) * 12);
  if (message) {
    strcpy(message, "hello world");
  }
  pthread_detach(pthread_self());
  return message;
}

int main() {
  pthread_t tid;
  int err;

  err = pthread_create(&tid, NULL, thread_function, NULL);
  if (err) {
    printf("Main thread: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  printf("Main thread: Created thread with ID %ld\n", (long)tid);

  pthread_exit(NULL);
}
