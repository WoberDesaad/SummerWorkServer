#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "BlockingQueue.h"

struct actor_queues{
	BlockingQueue* handle_client_q;
	BlockingQueue* log_q;
	BlockingQueue* file_q;
};

typedef struct actor_queues ActorQueues;

#endif
