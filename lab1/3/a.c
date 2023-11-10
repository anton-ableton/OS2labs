#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct MyStruct {
  int intValue;
  char *stringValue;
};

void *thread_function(void *arg) {
  struct MyStruct *data = (struct MyStruct *)arg;
  
  printf("Thread: intValue = %d, stringValue = %s\n", data->intValue, data->stringValue);

  return NULL;
}

int main() {
  pthread_t tid; 

  struct MyStruct myData;
  myData.intValue = 42;
  myData.stringValue = "Hello, Thread!";

  int result = pthread_create(&tid, NULL, thread_function, &myData);
  if (result != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  result = pthread_join(tid, NULL);
  if (result != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  return 0;
}
