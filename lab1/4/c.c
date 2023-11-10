#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

char *message = NULL;


void cleanup_handler(void *arg) {
  if (message != NULL) {
    free(message);
    message = NULL;
  }
}

void *thread_function(void *arg) {

  pthread_cleanup_push(cleanup_handler, NULL);

  message = (char *)malloc(12);
  if (message == NULL) {
    perror("malloc");
    pthread_exit(NULL);
  }
  strcpy(message, "hello world");

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); // the thread can be canceled by other threads using pthread_cancel().

  while (1) {
    printf("%s\n", message);
    sleep(1);
    pthread_testcancel(); 
  }

  // Note that we have finished with the memory release
  pthread_cleanup_pop(1);

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

  printf("Thread has been canceled and joined.\n");

  return 0;
}
