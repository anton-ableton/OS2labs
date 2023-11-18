#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

// Structure to represent a node in the queue
typedef struct _QueueNode {
	int val;                 // Value stored in the node
	struct _QueueNode *next; // Pointer to the next node
} qnode_t;

// Structure to represent a queue
typedef struct _Queue {
	qnode_t *first;         // Pointer to the first node in the queue
	qnode_t *last;          // Pointer to the last node in the queue
	pthread_t qmonitor_tid; // Thread ID of the monitoring thread
	int count;              // Current count of elements in the queue
	int max_count;          // Maximum allowed count in the queue
	// Queue statistics
	long add_attempts;      // Total number of add attempts
	long get_attempts;      // Total number of get attempts
	long add_count;         // Count of successful additions
	long get_count;         // Count of successful retrievals
} queue_t;


queue_t* queue_init(int max_count);     // Function to initialize a queue with a specified maximum count
void queue_destroy(queue_t *q);					// Function to destroy a queue and its associated resources
int queue_add(queue_t *q, int val);			// Function to add an element to the queue

// Function to retrieve an element from the queue
int queue_get(queue_t *q, int *val);

// Function to print queue statistics
void queue_print_stats(queue_t *q);

#endif   // __FITOS_QUEUE_H__
