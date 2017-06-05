#ifndef _FILE_ACTOR_H_
#define _FILE_ACTOR_H_

#include "BlockingQueue.h"

struct file_actor_request{
	struct http_request* file_req;
	BlockingQueue* return_queue;
};

struct file_return{
	int size;
	char* data;
};

#endif
