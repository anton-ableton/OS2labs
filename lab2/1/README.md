# queue.c 
## Queue Data Structures

### `queue_t`

- `queue_t` represents a queue and contains the following fields:
    - `first`: Pointer to the first node in the queue.
    - `last`: Pointer to the last node in the queue.
    - `qmonitor_tid`: Thread ID of the monitoring thread.
    - `count`: Current count of elements in the queue.
    - `max_count`: Maximum allowed count in the queue.
    - `add_attempts`: Total number of add attempts.
    - `get_attempts`: Total number of get attempts.
    - `add_count`: Count of successful additions.
    - `get_count`: Count of successful retrievals.

### `qnode_t`

- `qnode_t` represents a node in the queue and contains the following fields:
    - `val`: Value stored in the node.
    - `next`: Pointer to the next node.

## Function: `queue_init`

- Initializes a queue with a specified maximum count.
- Allocates memory for a `queue_t` structure and initializes its fields.
- Creates a monitoring thread using `pthread_create`.

## Function: `queue_destroy`

- Destroys a queue and releases associated resources.
- Cancels the monitoring thread using `pthread_cancel`.
- Waits for the monitoring thread to finish using `pthread_join`.
- Frees memory occupied by queue nodes and the `queue_t` structure.

## Function: `queue_add`

- Adds an element to the queue.
- Checks if the queue is not at its maximum capacity.
- Allocates memory for a new node and updates the pointers in the queue.

## Function: `queue_get`

- Retrieves an element from the queue.
- Checks if the queue is not empty.
- Removes the first node, retrieves the value, and frees the node.

## Function: `queue_print_stats`

- Prints statistics about the queue:
    - Current queue size.
    - The total number of add and get attempts.
    - The count of successful additions and retrievals.

## Function: `qmonitor`

- A monitoring function that runs in a separate thread.
- Periodically prints queue statistics.

## Compiler Flags

- `_GNU_SOURCE`: Enables GNU extensions for better compatibility.
- `pthread.h`: Header for POSIX threads.
- `assert.h`: Header for assertions.

The code provides a simple implementation of a queue and a monitoring thread to track statistics about the queue's operation.

# queue-threads.c

This C code demonstrates a multi-threaded program using a queue and processor affinity to manage the reader and writer threads.

## Queue and Thread Setup

### `queue_t`

- `queue_t` represents a queue data structure with various fields to manage statistics and data.
- The code uses the functions `queue_init`, `queue_add`, `queue_get`, and `queue_print_stats` to initialize and operate on the queue.

### `set_cpu`

- `set_cpu` is a function used to set the CPU affinity of a thread to a specific CPU core. It utilizes the `pthread_setaffinity_np` function for this purpose. In the context of CPU kernel management in multitasking systems, "affinity" refers to the ability to bind (bind) the execution of a particular thread or process to a specific CPU kernel.

## Reader Thread

### `reader`

- `reader` is a function representing the reader thread.
- It continuously reads values from the queue and checks their sequence.
- If a mismatch is found, it prints an error message in red.

## Writer Thread

### `writer`

- `writer` is a function representing the writer thread.
- It continuously writes values to the queue.

## `main` Function

- The `main` function is the program's entry point.
- It creates a queue, reader, and writer threads using `queue_init` and `pthread_create`.
- It assigns CPU cores using `set_cpu` for reader and writer threads to improve performance.
- Both reader and writer threads run concurrently to test the queue operations.
- Note that the code currently lacks the joining of threads.

### `RED` and `NOCOLOR`

- These are color codes for printing text in red and resetting the text color to default.

## Compiler Flags

- `_GNU_SOURCE`: Enables GNU extensions for better compatibility.
- `pthread.h`: Header for POSIX threads.
- `sched.h`: Header for scheduling functions and CPU affinity.

The code demonstrates a multi-threaded scenario with separate reader and writer threads that communicate through a queue. The writer thread continuously adds values to the queue, while the reader thread checks for any discrepancies in the sequence of values. The use of processor affinity (`set_cpu`) aims to optimize CPU core usage.
