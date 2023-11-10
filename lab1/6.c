// Compilation: gcc -o 6 6.c -lpthread

#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

typedef struct {
  pthread_t tid;
  int exited;
} mythread_t;

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {
  size_t size = 65536;
  void *stack = malloc(size);
  if (stack == NULL) {
    perror("mythread_create: malloc failed");
    return -1;
  }

  int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
  int result = clone(start_routine, stack + size, flags, arg);

  if (result == -1) {
    perror("mythread_create: clone failed");
    free(stack);
    return -1;
  }

  thread->tid = result;
  thread->exited = 0;

  return 0;
}

void *mythread_function(void *arg) {
  // The function the thread will perform
  printf("Thread: Hello from mythread_function!\n");
  ((mythread_t *)arg)->exited = 1;
  return NULL;
}

void *mythread2_function(void *arg) {
  // The function the thread will perform
  printf("Thread2: Hello from mythread_function!\n");
  ((mythread_t *)arg)->exited = 1;
  return NULL;
}

int main() {
  mythread_t thread, thread2;

  if (mythread_create(&thread, mythread_function, &thread) == -1) {
    fprintf(stderr, "Failed to create thread\n");
    return 1;
  }

  if (mythread_create(&thread2, mythread2_function, &thread2) == -1) {
    fprintf(stderr, "Failed to create thread\n");
    return 1;
  }

  while (!thread.exited) {}
  while (!thread2.exited) {}

  return 0;
}
