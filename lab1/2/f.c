//gcc f.c -o f -pthread

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *thread_function(void *arg) {
  char *message = "hello world";
  return message;
}

int main() {
  pthread_t tid; 
  int err;

  pthread_attr_t attr;
  err = pthread_attr_init(&attr);
  if (err) {
    printf("Main thread: pthread_attr_init() failed: %s\n", strerror(err));
    return -1;
  }

  err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (err) {
    printf("Main thread: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
    return -1;
  }

  err = pthread_create(&tid, &attr, thread_function, NULL);
  if (err) {
    printf("Main thread: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  pthread_attr_destroy(&attr);


  pthread_exit(NULL);
}
