#define _GNU_SOURCE

#include <sys/syscall.h>
#include <linux/futex.h>
#include "queue.h"

typedef struct
{
    int lock; // 1 - unlock, 0 - lock
} mutex_t;

mutex_t *mutex;

void mutex_init(mutex_t *m)
{
    m->lock = 1;
}

// An auxiliary function for calling a futex system call. FUTEX_WAIT blocks the stream until
// the value the uaddr points to is 0. FUTEX_WAKE awakens one or more threads waiting for the value change at the uaddr address.
static int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr, val3);
}

void mutex_lock(mutex_t *m)
{
    while (1)
    {
        int one = 1;
        // trying to set m->lock to 0 with atomic_compare_exchange_strong atomic_operation.
        // If the operation fails, FUTEX_WAIT is called to block the stream until the m->lock
        // value is 0. If the FUTEX_WAIT fails, the error code is checked, and if it is not an
        // EAGAIN (meaning that the error is linked to the wait condition)program fails, the program fails.
        if (atomic_compare_exchange_strong(&m->lock, &one, 0))
        {
            break;
        }
        int err = futex(&m->lock, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (err == -1 && errno != EAGAIN)
        {
            printf("Futex wait failed");
            abort();
        }
    }
}

void mutex_unlock(mutex_t *m)
{
    int zero = 0;
    // trying to install m->lock in 1 with atomic_compare_exchange_strong. If the operation is successful,
    // FUTEX_WAKE is called to wake up one or more threads waiting for the m->lock value to change.
    if (atomic_compare_exchange_strong(&m->lock, &zero, 1))
    {
        int err = futex(&m->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (err == -1)
        {
            printf("Futex wake failed");
            abort();
        }
    }
}

void *qmonitor(void *arg)
{
    queue_t *q = (queue_t *)arg;

    printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1)
    {
        queue_print_stats(q);
        sleep(1);
    }

    return NULL;
}

queue_t *queue_init(int max_count)
{

    int err;
    mutex = malloc(sizeof(mutex_t));
    mutex_init(mutex);
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
    {
        printf("Cannot allocate memory for a queue\n");
        abort();
    }

    q->first = NULL;
    q->last = NULL;
    q->max_count = max_count;
    q->count = 0;

    q->add_attempts = q->get_attempts = 0;
    q->add_count = q->get_count = 0;

    err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
    if (err)
    {
        printf("queue_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    return q;
}

void queue_destroy(queue_t *q)
{
    if (q == NULL)
    {
        return;
    }

    pthread_cancel(q->qmonitor_tid);
    pthread_join(q->qmonitor_tid, NULL);

    free(mutex);

    qnode_t *current = q->first;
    while (current != NULL)
    {
        qnode_t *temp = current;
        current = current->next;
        free(temp);
    }

    q->count = 0;
    q->first = NULL;
    q->last = NULL;

    free(q);
}

int queue_add(queue_t *q, int val)
{
    mutex_lock(mutex);
    q->add_attempts++;

    assert(q->count <= q->max_count);

    if (q->count == q->max_count)
    {
        mutex_unlock(mutex);
        return 0;
    }

    qnode_t *new = malloc(sizeof(qnode_t));
    if (!new)
    {
        printf("Cannot allocate memory for new node\n");
        mutex_unlock(mutex);
        abort();
    }

    new->val = val;
    new->next = NULL;

    if (!q->first)
        q->first = q->last = new;
    else
    {
        q->last->next = new;
        q->last = q->last->next;
    }
    q->count++;
    q->add_count++;
    mutex_unlock(mutex);

    return 1;
}

int queue_get(queue_t *q, int *val)
{
    mutex_lock(mutex);
    q->get_attempts++;

    assert(q->count >= 0);

    if (q->count == 0)
    {
        mutex_unlock(mutex);
        return 0;
    }

    qnode_t *tmp = q->first;
    *val = tmp->val;

    q->first = q->first->next;

    free(tmp);
    q->count--;
    q->get_count++;
    mutex_unlock(mutex);

    return 1;
}

void queue_print_stats(queue_t *q)
{
    printf("queue stats: current size %d; attempts: (%ld %ld (dif:)%ld); counts (%ld %ld (dif:)%ld)\n",
           q->count,
           q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
           q->add_count, q->get_count, q->add_count - q->get_count);
}
