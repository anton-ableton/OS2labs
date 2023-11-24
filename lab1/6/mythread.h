#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pthread.h>

typedef void* (*start_routine_t)(void*);

typedef struct _mythread {
    int mythread_id;
    void* arg;
    start_routine_t start_routine;
    void* retval;
    volatile int joined;
    volatile int finished;
} mythread_struct_t;

typedef mythread_struct_t* mythread_t;

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg);

#endif /* MYTHREAD_H */
