#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct MyStruct
{
	int intValue;
	char *stringValue;
};

void *thread_function(void *arg)
{
	struct MyStruct *data = (struct MyStruct *) arg;

	printf("Thread: intValue = %d, stringValue = %s\n", data->intValue, data->stringValue);

	free(arg);

	return NULL;
}

int main()
{
	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	struct MyStruct *myData = (struct MyStruct *) malloc(sizeof(struct MyStruct));

	if (myData == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	myData->intValue = 42;
	myData->stringValue = "Hello, Thread!";

	int result = pthread_create(&tid, &attr, thread_function, myData);
	if (result != 0)
	{
		perror("pthread_create");
		free(myData);
		exit(EXIT_FAILURE);
	}

	pthread_attr_destroy(&attr);

	pthread_exit(NULL);
}
