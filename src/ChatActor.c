#include "ChatActor.h"
#include "BlockingQueue.h"

#define MAX_CHATTERS 20

//Globals Lock before using
long chatters[MAX_CHATTERS];


//Wait on Queue to add new chatters
void* ChatActor(void* arg){
	//Actor Stuff
	ActorQueues* Q = arg;
	
	//Local Variables	
	//file descriptors for sockets objects
  	int remfd; 
 	int err;
  	
  	//I/O Message Stuff
	char mess_buff[8192];
	char str_buff[256];
	char submit_buff[1024];
	char rec_buff[8192];
 	int mess_len = 0;


	//Random Stuff
	int c;
	char* cptr_start;
	void* conn_ptr;
	void* log_message;
	
		
	//Logging Stuff
	char log_buffer[1024];
	int log_len;

	ActorQueues* Q = arg;
	struct connection_data* client;
	
	while(1){
		bzero(&str_buff, 256);
		bzero(&mess_buff, 8192);
		bzero(&rec_buff, 8192);
		
		conn_ptr = BlockingQueue_remove(Q->handle_client_q);
	}
}

//Select on all current chatters and refresh list every second
void ChatWatcher(void* arg){
	//Actor Stuff
	ActorQueues* Q = arg;
	
	//File stuff
	FILE *chatFile;
	
	int i, maxfd;
	
	while(1){
		//reset for selecting
		maxfd = 0;
		for(i = 0; i < NUM_CHATTERS; i++){
			if(chatters[i] != 0){
				if(chatters[i] > maxfd){
					maxfd = chatters[i];
				}
				//add fd to set
			}
		}
		
		
		//select
		
		
	}
}