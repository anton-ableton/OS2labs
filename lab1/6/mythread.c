#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mythread.h"

#define PAGE 4096
#define STACK_SIZE 3 * PAGE


int thread_startup(void* arg)
{

    mythread_t tid = (mythread_t)arg;
    mythread_struct_t* thread = tid;
    void* retval;

    printf("thread_startup: starting a thread function for the thread %d\n", thread->mythread_id);
    retval = thread->start_routine(thread->arg);

    thread->retval = retval;
    thread->finished = 1;

    if (thread->joinable) {
        printf("thread_startup: waiting for join() the thread %d\n", thread->mythread_id);
        while (!thread->joined)
        usleep(1);
    }

    printf("thread_startup: the thread function finished for the thread %d\n", thread->mythread_id);

    return 0;
}

void* create_stack(off_t size, int mytid)
{
    char stack_file[128];
    int stack_fd;
    void* stack;

    snprintf(stack_file, sizeof(stack_file), "stack-%d", mytid);
    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    ftruncate(stack_fd, 0);

    ftruncate(stack_fd, size);

    stack = mmap(NULL, size, PROT_NONE, MAP_SHARED, stack_fd, 0);
    close(stack_fd);

    return stack;
}

int mythread_create(mythread_t* mytid, int is_joinable, void* (*start_routine)(void*), void* arg) {
    static int mythread_id = 0;
    mythread_struct_t* thread;
    pid_t child_pid;
    void* child_stack;

    mythread_id++;

    printf("mythread_create: creating thread %d\n", mythread_id);
    child_stack = create_stack(STACK_SIZE, mythread_id);
    mprotect(child_stack + PAGE, STACK_SIZE - PAGE, PROT_READ | PROT_WRITE);
    memset(child_stack + PAGE, 0x7f, STACK_SIZE - PAGE);

    thread = (mythread_struct_t*)(child_stack + STACK_SIZE - sizeof(mythread_struct_t));
    thread->mythread_id = mythread_id;

    thread->arg = arg;

    thread->start_routine = start_routine;

    thread->retval = NULL;

    thread->finished = 0;

    thread->joined = 0;

    thread->joinable = is_joinable;

    child_stack = (void*)thread;

    printf("child stack %p; mythread_struct %p; \n", child_stack, thread);

    

    if (!is_joinable) {
        child_pid = clone(thread_startup, child_stack, CLONE_VM | CLONE_SIGHAND | SIGCHLD, thread);
        int status;
        waitpid(child_pid, &status, 0);
    }
    else {
        child_pid = clone(thread_startup, child_stack, CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | SIGCHLD, thread);
    }

    *mytid = thread;

    return 0;
}


void mythread_join(mythread_t mytid, void** retval)
{
    mythread_struct_t* thread = mytid;

    printf("thread_join: waiting for the thread %d to finish\n", thread->mythread_id);

    // microseconds
    while (!thread->finished)
        usleep(1);

    printf("thread_join: the thread %d finished\n", thread->mythread_id);

    *retval = thread->retval;
    thread->joined = 1;
}

