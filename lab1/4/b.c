#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

volatile int counter = 0;

void *thread_function(void *arg) {
  while (1) {
    pthread_testcancel(); // Пользовательская точка отмены
    counter++;
  }

  return NULL;
}

int main() {
  pthread_t tid; 

  int result = pthread_create(&tid, NULL, thread_function, NULL);
  if (result != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  sleep(5);

  result = pthread_cancel(tid);
  if (result != 0) {
    perror("pthread_cancel");
    exit(EXIT_FAILURE);
  }

  result = pthread_join(tid, NULL);
  if (result != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  printf("Thread has been canceled and joined. Counter value: %d\n", counter);

  return 0;
}
