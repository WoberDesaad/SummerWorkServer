//Name: Robert Appelmann
#include "HandleClientActor.h"
#include "BlockingQueue.h"

extern char* quit;

char* log_file = "../resources/log.txt";

void* handle_log_actor(void *arg){
	//Local Variables	
	//file descriptors for sockets objects
  	
	//File stuff
	FILE *logFile;
		
	//Logging Stuff
	char* log_ptr;
	char time_buff[256];
	struct tm * tmtime;
	time_t t;
	
	ActorQueues* Q = arg;
	
	t = time(NULL);
	tmtime = localtime(&t);
	strftime(time_buff, 256, "%T, %A, %B %d %Y", tmtime);
	
	logFile = fopen(log_file, "a");
	
	fprintf(logFile, "Starting logging...%s\n", time_buff);
	
	fclose(logFile);
	
	while(1){
		log_ptr = BlockingQueue_remove(Q->log_q);
		
		t = time(NULL);
		tmtime = localtime(&t);
		strftime(time_buff, 256, "%T, %A, %B %d %Y", tmtime);
		
		if(strlen(log_ptr) >= 4)
		{
			if(log_ptr[0] == 'Q' && log_ptr[1] == 'U' && log_ptr[2] == 'I' && log_ptr[3] == 'T')
			{
				break;
			}
		}
		//printf("%s: ", time_buff);
		//printf("%s\n", log_ptr);
		
		logFile = fopen(log_file, "a");
		
		fprintf(logFile, "%s: %s\n", time_buff, log_ptr);

		fclose(logFile);
		
		bzero(time_buff, 256);
		free(log_ptr);
	}
	printf("Logging Thread Done!\n");
	
	return NULL;
}

