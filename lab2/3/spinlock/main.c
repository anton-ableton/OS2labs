#include <unistd.h>
#include <pthread.h>
#include "queue.h"

#define STORAGE_CAPACITY 1000
#define THREAD_COUNT 6
#define ASC 0
#define DESC 1
#define EQ 2
#define SWAP1 3
#define SWAP2 4
#define SWAP3 5

int compare_nodes(const char *value1, const char *value2, int operation)
{
  int pair_count = 0;
  if ((operation == ASC && strlen(value1) < strlen(value2)) ||
      (operation == DESC && strlen(value1) > strlen(value2)) ||
      (operation == EQ && strlen(value1) == strlen(value2)))
  {
    pair_count++;
  }
  return pair_count;
}

void *process_thread(void *data)
{
  ThreadData *thread_data = (ThreadData *)data;
  Storage *storage = thread_data->storage;
  int *counter = thread_data->counter;
  if (storage->first == NULL || storage->first->next == NULL)
  {
    printf("Too few elements in queue\n");
    return NULL;
  }

  while (1)
  {
    Node *curr = storage->first;
    if (curr == NULL || curr->next == NULL)
    {
      printf("Too few elements in queue to asc\n");
      break;
    }
    Node *curr2, *tmp;
    while (1)
    {
      if (curr != NULL && pthread_spin_trylock(&curr->spinlock) == 0)
      {
        if (curr->next != NULL && pthread_spin_trylock(&curr->next->spinlock) == 0)
        {
          volatile int pair_count = 0;
          curr2 = curr->next;
          int result = compare_nodes(curr->value, curr2->value, counter);
          pair_count += result;
          tmp = curr;
          curr = curr->next;
          pthread_spin_unlock(&tmp->spinlock);
          pthread_spin_unlock(&curr->spinlock);
        }
        else
        {
          tmp = curr;
          curr = curr->next;
          pthread_spin_unlock(&tmp->spinlock);
        }
      }
      else if (curr == NULL)
      {
        break;
      }
      else
      {
        curr = curr->next;
      }
    }
    (*counter)++;
  }
  return NULL;
}

void *swap_thread(void *data)
{
  ThreadData *thread_data = (ThreadData *)data;
  Storage *storage = thread_data->storage;
  int *counter = thread_data->counter;

  while (1)
  {
    Node *curr1 = storage->first;
    if (curr1 == NULL || curr1->next == NULL || curr1->next->next == NULL)
    {
      printf("Too few elements in queue to swap\n");
      break;
    }
    Node *curr2, *curr3, *tmp;
    int should_swap = rand() % 2;
    while (1)
    {
      if (should_swap == 0)
      {
        if (curr1->next != NULL)
        {
          curr1 = curr1->next;
          continue;
        }
        else
          break;
      }
      if (curr1 != NULL && pthread_spin_trylock(&curr1->spinlock) == 0)
      {
        if (curr1->next != NULL && pthread_spin_trylock(&curr1->next->spinlock) == 0)
        {
          if (curr1->next->next != NULL && pthread_spin_trylock(&curr1->next->next->spinlock) == 0)
          {
            curr2 = curr1->next;
            curr3 = curr1->next->next;
            curr2->next = curr3->next;
            curr3->next = curr2;
            curr1->next = curr3;
            (*counter)++;
            tmp = curr1;
            curr1 = tmp->next;
            curr2 = curr1->next;
            pthread_spin_unlock(&tmp->spinlock);
            pthread_spin_unlock(&curr1->spinlock);
            pthread_spin_unlock(&curr2->spinlock);
          }
          else
          {
            tmp = curr1;
            curr1 = curr1->next;
            pthread_spin_unlock(&tmp->spinlock);
            pthread_spin_unlock(&curr1->spinlock);
          }
        }
        else
        {
          tmp = curr1;
          curr1 = curr1->next;
          pthread_spin_unlock(&tmp->spinlock);
        }
      }
      else if (curr1 == NULL)
      {
        break;
      }
      else
      {
        curr1 = curr1->next;
      }
    }
  }
}

void *count_monitor(void *arg)
{
  int *counters = (int *)arg;
  while (1)
  {
    printf("ASC: %d, DESC: %d, EQ: %d, SWAP1: %d, SWAP2: %d, SWAP3: %d\n",
           counters[ASC], counters[DESC], counters[EQ], counters[SWAP1], counters[SWAP2], counters[SWAP3]);
    sleep(1);
  }
  return NULL;
}

int main()
{
  Storage *storage = initialize_storage(STORAGE_CAPACITY);
  fill_storage(storage);

  pthread_t ascending_tid, descending_tid, equal_length_tid, swap_tid1, swap_tid2, swap_tid3, monitor;

  int *counters = calloc(THREAD_COUNT, sizeof(int));

  ThreadData ascending_data = {storage, &counters[ASC]};
  ThreadData descending_data = {storage, &counters[DESC]};
  ThreadData equal_data = {storage, &counters[EQ]};
  ThreadData swap_data1 = {storage, &counters[SWAP1]};
  ThreadData swap_data2 = {storage, &counters[SWAP2]};
  ThreadData swap_data3 = {storage, &counters[SWAP3]};

  pthread_create(&ascending_tid, NULL, process_thread, &ascending_data);
  pthread_create(&descending_tid, NULL, process_thread, &descending_data);
  pthread_create(&equal_length_tid, NULL, process_thread, &equal_data);
  pthread_create(&swap_tid1, NULL, swap_thread, &swap_data1);
  pthread_create(&swap_tid2, NULL, swap_thread, &swap_data2);
  pthread_create(&swap_tid3, NULL, swap_thread, &swap_data3);
  pthread_create(&monitor, NULL, count_monitor, counters);

  pthread_join(ascending_tid, NULL);
  pthread_join(descending_tid, NULL);
  pthread_join(equal_length_tid, NULL);
  pthread_join(monitor, NULL);
  pthread_join(swap_tid1, NULL);
  pthread_join(swap_tid2, NULL);
  pthread_join(swap_tid3, NULL);

  return 0;
}
