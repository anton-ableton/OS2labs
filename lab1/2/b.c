//gcc b.c -o b -pthread

#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg) {
  int *result = malloc(sizeof(int));
  *result = 42;
  return result;
}

int main() {
  pthread_t tid; 
  int err;

  err = pthread_create(&tid, NULL, thread_function, NULL);
  if (err) {
    printf("Main thread: pthread_create() failed: %s\n", strerror(err));
    return -1;
  }

  int *thread_result;

  err = pthread_join(tid, (void **)&thread_result);
  if (err) {
    printf("Main thread: pthread_join() failed: %s\n", strerror(err));
    return -1;
  }

  printf("Main thread: Child thread returned %d\n", *thread_result);

  free(thread_result);

  return 0;
}
