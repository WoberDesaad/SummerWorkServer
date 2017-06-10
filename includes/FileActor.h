#ifndef _FILE_ACTOR_H_
#define _FILE_ACTOR_H_

#include "BlockingQueue.h"
#include "Actor.h"
#include "HandleClientActor.h"

struct file_actor_request{
	struct http_request* file_req;
	BlockingQueue* return_queue;
};

struct file_return{
	int size;
	char* data;
};

void* file_actor(void* arg);
void return_data(BlockingQueue* Q, char* data, int ret_len);

#endif
