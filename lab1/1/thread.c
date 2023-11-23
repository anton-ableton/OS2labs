
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void *mythread(void *arg)
{
	//info about process (PID), parent process (PPID) and stream identifier (TID)
	printf("mythread[%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
	return NULL;
}

int main()
{
	// the data type used to represent the identifier
	pthread_t tid;
	int err;

	printf("main[%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	// 1) id 2) attributes 3) pointer to function 4) function arg 
	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err)
	{
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

	sleep(30);

	return 0;
}
