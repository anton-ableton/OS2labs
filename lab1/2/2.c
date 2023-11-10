#include <stdio.h>
#include <pthread.h>
void *thread_function(void *arg) {
  printf("Child thread: Hello from child thread!\n");
  return NULL;
}

int main() {
  pthread_t tid;
  int err;
  
  err = pthread_create(&tid, NULL, thread_function, NULL);
  if (err) {
    printf("Main thread: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  err = pthread_join(tid, NULL);
  if (err) {
    printf("Main thread: pthread_join() failed: %s\n", strerror(err));
    return -1;
  }

  printf("Main thread: Child thread has been joined and completed.\n");

  return 0;
}
