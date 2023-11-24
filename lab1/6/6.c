// Compilation: gcc -o 6 6.c -lpthread

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "mythread.h"


void* mythread(void* arg)
{
    char* str = (char*)arg;

    for (int i = 0; i < 10; i++) {
        printf("hello from my thread ‘%s'\n", str);
        sleep(1);
    }

    return "bay";
}

int main() {
    mythread_t mytid;
    void* retval;
    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());
    mythread_create(&mytid, mythread, "hello from main");
    mythread_join(mytid, &retval);
    printf("main [%d %d %d] thread returned '%s'\n", getpid(), getppid(), gettid(), (char*)retval);

    return 0;
}
