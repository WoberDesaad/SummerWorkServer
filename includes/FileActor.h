#include "BlockingQueue.h"

struct file_actor_request{
	char* file_name;
	char* buffer;
	int buffer_size;
	int offset;
	BlockingQueue* return_queue;
	int num_return;
};