// Compilation: gcc -o 6 6.c mythread.c -lpthread

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "mythread.h"


void* mythread(void* arg)
{
    char* str = (char*)arg;

    for (int i = 0; i < 4; i++) {
        printf("hello from my thread ‘%s'\n", str);
        sleep(1);
    }

    return "bay";
}

int main() {
    mythread_t mytid;
    void* retval;
    int err;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

    int is_joinable = 0;

    err = mythread_create(&mytid, is_joinable, mythread, "hello from main");
    if (err) {
        printf("Main thread: mythread_create() for Thread 3 failed\n");
        return -1;
    }
    
    // mythread_join(mytid, &retval);
    printf("main [%d %d %d] thread returned '%s'\n", getpid(), getppid(), gettid(), (char*)retval);
    return 0;
}
