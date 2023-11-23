// gcc d.c -o d -pthread
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *thread_function(void *arg) {
  pthread_t tid = pthread_self(); 
  printf("Thread %ld: Hello from thread!\n", tid);
  pthread_exit(NULL); 
}

int main() {
  while (1) {
    int err;
    pthread_t tid;
    pthread_attr_t attr;
    
    err = pthread_attr_init(&attr);
    if (err) {
      printf("Main thread: pthread_attr_init() failed: %d\n", strerror(err));
      return -1;
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (err) {
      printf("Main thread: pthread_attr_setdetachstate() failed: %d\n", strerror(err));
      return -1;
    }

    err = pthread_create(&tid, &attr, thread_function, NULL);
    if (err) {
      printf("Main thread: pthread_create() failed: %d\n", strerror(err));
      return -1;
    }

    pthread_attr_destroy(&attr);

    err = pthread_join(tid, NULL);
    if (err) {
      printf("Main thread: pthread_join() failed: %d\n", strerror(err));
      return -1;
    }
  }

  return 0;
}
