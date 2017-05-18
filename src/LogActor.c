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
	char time_buff[256];
	struct tm * tmtime;
	
	ActorQueues* Q = arg;
	
	time_t t = time(NULL);
	tmtime = localtime(&t);
	strftime(time_buff, 256, "%T, %A, %B %d %Y", tmtime);
	
	logFile = fopen(log_file, "a");
	
	fprintf(logFile, "Starting logging...%s", time_buff);
	
	fclose(logFile);
	
	while(1){
		log_ptr = BlockingQueue_remove(Q->log_q);
		
		t = time(NULL);
		tmtime = localtime(&t);
		strftime(time_buff, 256, "%T, %A, %B %d %Y", tmtime);
		
		if(log_ptr[0] == 'Q' && log_ptr[1] == 'U' && log_ptr[2] == 'I' && log_ptr[3] == 'T')
		{
			break;
		}
	
		printf("%s: ", time_buff);
		printf("%s\n", log_ptr);
		
		log_len = strlen(log_ptr);
		
		logFile = fopen(log_file, "a");
		
		fprintf(logFile, "%s: %s\n", time_buff, log_ptr);

		fclose(logFile);
		
		bzero(log_ptr, log_len);
		bzero(time_buff, 256);
		free(log_ptr);
	}
	printf("Logging Thread Done!\n");
	
	return NULL;
}



