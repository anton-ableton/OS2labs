#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define NUM_THREADS 5

int globalVariable = 42;

void *mythread(void *arg)
{
	int localVar = 99;

	static int staticLocalVar = 77;

	const int constLocalVar = 123;

	// globalVariable = 24;
	// localVar = 66;

	printf("mythread[PID: %d, PPID: %d, TID(p_s): %ld, TID(gettid): %ld]:\n", getpid(), getppid(), pthread_self(), gettid());

	printf("Local Variable Address: %p\n", (void*) &localVar);
	printf("Static Local Variable Address: %p\n", (void*) &staticLocalVar);
	printf("Constant Local Variable Address: %p\n", (void*) &constLocalVar);
	printf("Global Variable Address: %p\n", (void*) &globalVariable);

	if (pthread_equal(pthread_self(), *((pthread_t*) arg)))
	{
		printf("pthread_self mythread: Thread IDs match!\n");
	}
	else
	{
		printf("pthread_self mythread: Thread IDs do not match.\n");
	}

	if (pthread_equal(gettid(), *((pthread_t*) arg)))
	{
		printf("gettid mythread: Thread IDs match!\n");
	}
	else
	{
		printf("gettid mythread: Thread IDs do not match.\n");
	}

	return NULL;
}

int main()
{
	pthread_t tid[NUM_THREADS];
	int err;

	printf("main[PID: %d, PPID: %d, TID(p_s): %ld]: Hello from main!\n", getpid(), getppid(), pthread_self());

	for (int i = 0; i < NUM_THREADS; i++)
	{
		err = pthread_create(&tid[i], NULL, mythread, &tid[i]);
		if (err)
		{
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return -1;
		}
	}

	sleep(3);

	for (int i = 0; i < NUM_THREADS; i++)
	{
		err = pthread_join(tid[i], NULL);
		if (err)
		{
			printf("main: pthread_join() failed: %s\n", strerror(err));
			return -1;
		}
	}

	return 0;
}
