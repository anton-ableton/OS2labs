#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signo) {
  const char* msg = "Int\n";
  write(STDOUT_FILENO, msg, strlen(msg)); 
}

void *thread2_function(void *arg) {
  signal(SIGINT, sigint_handler);

  while (1) {}
  return NULL;
}

void sigquit_handler(int signo) {
  const char* msg = "Quit\n";
  write(STDOUT_FILENO, msg, strlen(msg));
}

void *thread3_function(void *arg) {
  int sig;
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGQUIT);

  sigwait(&set, &sig);
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
  sigfillset(&all_signals);
  pthread_sigmask(SIG_BLOCK, &all_signals, NULL);

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
