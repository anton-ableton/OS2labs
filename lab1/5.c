//gcc 5.c -o 5 -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void sigint_handler(int signo) {
  const char *msg = "Int\n";
  write(STDOUT_FILENO, msg, strlen(msg));
}

void *thread2_function(void *arg) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigint_handler;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("Main thread: sigaction() for SIGINT failed");
    return NULL;
  }

  while (1) {}
  return NULL;
}

void sigquit_handler(int signo) {
  const char *msg = "Quit\n";
  write(STDOUT_FILENO, msg, strlen(msg));
}

void *thread3_function(void *arg) {
  int sig;
  sigset_t set;

  if (sigemptyset(&set) == -1) {
    perror("Main thread: sigemptyset() failed");
    return NULL;
  }

  if (sigaddset(&set, SIGQUIT) == -1) {
    perror("Main thread: sigaddset() for SIGQUIT failed");
    return NULL;
  }

  if (sigwait(&set, &sig) != 0) {
    perror("Main thread: sigwait() failed");
    return NULL;
  }

  sigquit_handler(sig);
  return NULL;
}

int main() {
  pthread_t tid2, tid3;
  int err;

  printf("Main thread: Hello from main!\n");

  err = pthread_create(&tid2, NULL, thread2_function, NULL);
  if (err) {
    printf("Main thread: pthread_create() for Thread 2 failed\n");
    return -1;
  }

  err = pthread_create(&tid3, NULL, thread3_function, NULL);
  if (err) {
    printf("Main thread: pthread_create() for Thread 3 failed\n");
    return -1;
  }

  sigset_t all_signals;
  if (sigfillset(&all_signals) == -1) {
    perror("Main thread: sigfillset() failed");
    return -1;
  }

  if (pthread_sigmask(SIG_BLOCK, &all_signals, NULL) == -1) {
    perror("Main thread: pthread_sigmask() failed");
    return -1;
  }

  err = pthread_join(tid2, NULL);
  if (err) {
    printf("Main thread: pthread_join() for Thread 2 failed\n");
    return -1;
  }

  err = pthread_join(tid3, NULL);
  if (err) {
    printf("Main thread: pthread_join() for Thread 3 failed\n");
    return -1;
  }

  return 0;
}
