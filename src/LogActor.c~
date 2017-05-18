//Name: Robert Appelmann
#include "HandleClientActor.h"
#include "BlockingQueue.h"

extern char* quit;

char* log_file = "log.txt";

void* handle_log_actor(void *arg){
	//Local Variables	
	//file descriptors for sockets objects
  	
	//File stuff
	FILE *logFile;
		
	//Logging Stuff
	char* log_ptr;
	int log_len;
	
	ActorQueues* Q = arg;
	
	time_t t = time(NULL);
	char* str_time = ctime(&t);
	
	logFile = fopen(log_file, "a");
	
	fprintf(logFile, "Starting logging...%s", str_time);
	
	fclose(logFile);
	
	while(1){
		log_ptr = BlockingQueue_remove(Q->log_q);
		if(log_ptr[0] == 'Q' && log_ptr[1] == 'U' && log_ptr[2] == 'I' && log_ptr[3] == 'T')
		{
			break;
		}
	
		printf("Logging: ");
		
		log_len = strlen(log_ptr);
		
		logFile = fopen(log_file, "a");
		
		fprintf(logFile, "%s\n", log_ptr);

		fclose(logFile);
		
		printf("%s\n", log_ptr);
		bzero(log_ptr, log_len);
		free(log_ptr);
	}
	printf("Logging Thread Done!\n");
	
	return NULL;
}



