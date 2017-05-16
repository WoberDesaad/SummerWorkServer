#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/**
 * We define a blocking queue data structure with a bound
 * on the number of entries. This data structure takes care
 * of its own synchronization internally. The add operation
 * blocks if the queue is full, and the remove operation
 * blocks if the queue is empty. Obviously it's FIFO.
 */

#define QUEUE_SIZE 1024

typedef struct BlockingQueueStruct BlockingQueue;

BlockingQueue *BlockingQueue_create();

void BlockingQueue_destroy(BlockingQueue *q);

void BlockingQueue_add(BlockingQueue *q, void *entry);

void *BlockingQueue_remove(BlockingQueue *q);

void *BlockingQueue_tryRemove(BlockingQueue *q);

void *BlockingQueue_timedRemove(BlockingQueue *q, uint32_t milliseconds);

#endif /* _BLOCKING_QUEUE_H_ */
