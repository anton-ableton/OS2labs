//gcc c.c -o c -pthread

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

  err = pthread_create(&tid, NULL, thread_function, NULL);
  if (err) {
    printf("Main thread: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  char *thread_result;

  err = pthread_join(tid, (void **)&thread_result);
  if (err) {
    printf("Main thread: pthread_join() failed: %s\n", strerror(err));
    return -1;
  }

  printf("Main thread: Child thread returned: %s\n", thread_result);

  return 0;
}
