//Name: Robert Appelmann
//Class: ETEC3201 Networking and Communications
//Program: Final Exam question 14
//Time Server
#include "Actors.h"
#include "HandleClientActor.h"
#include "LogActor.h"
#include "BlockingQueue.h"
#include "FileActor.h"
#include "Main.h"

//Globals
char* quit = "QUIT";

int main()
{
	//file descriptors for sockets objects
  	int sockfd, remfd, i, selectfd;
	
  	//Address stuff
 	struct sockaddr_in addr1;
 	struct sockaddr_in rem_addr; 
 	int err;
	unsigned int rem_addr_len;
 	char r, c;
 	fd_set fds;
 	void* connptr;
 	
	//actor package
	ActorQueues Q;

	//threads
	pthread_t handle_client_threads[NUM_HANDLE_CLIENT_ACTORS];
	pthread_t log_thread;
	pthread_t file_thread;
	
	Q.handle_client_q = BlockingQueue_create();
	Q.log_q = BlockingQueue_create();
	Q.file_q = BlockingQueue_create();

	//initialize addr to zero
	memset(&rem_addr, '\0', sizeof(struct sockaddr));

	//Create Threads
	for(i = 0; i < NUM_HANDLE_CLIENT_ACTORS; i++){
		pthread_create(&handle_client_threads[i], NULL, handle_client_actor, (void*)&Q);
	}
	
	pthread_create(&log_thread, NULL, handle_log_actor, (void*)&Q);
	pthread_create(&file_thread, NULL, file_actor, (void*)&Q);
	
	//Create Socket
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	if(sockfd < 0){
 		printf("%s\n", strerror(errno));
  		return -1;
 	}
  	printf("Socket Opened....");
 	
 	//Address Stuff
	addr1.sin_family = AF_INET;
	addr1.sin_port = htons( PORT1 );//htons ensures correct byte order
	addr1.sin_addr.s_addr = 0;//Tells bind to use the IP Address of the system
  	bzero(&(addr1.sin_zero), 8);


  	//Binding Sockets
	err = bind(sockfd, (struct sockaddr*)&addr1, sizeof(struct sockaddr));
  	if(err < 0){
 		printf("%s\n", strerror(errno));
  		return -1;
  	}
  	printf("Socket Bound\n");
  
  	//Listening
 	err = listen(sockfd,10);
  	if(err < 0){
 		printf("%s\n", strerror(errno));
  		return -1;
 	}
  	printf("Listening on port %d...\n\n", PORT1);
  

  	while(1){  
		printf("#> ");
	
		rem_addr_len = sizeof(struct sockaddr_in);

  		//Select
  		FD_ZERO(&fds);
  		FD_SET(sockfd, &fds);
  		FD_SET(0, &fds);
		
  		selectfd = select(sockfd+1, &fds, NULL, NULL, NULL);
  		
  		if(selectfd < 0){
  			//Error
  			printf("Error Selecting: %s\n", strerror(errno));
			for(i = 0; i < NUM_HANDLE_CLIENT_ACTORS; i++){
				BlockingQueue_add(Q.handle_client_q, quit);
			}
			
			for(i = 0; i < NUM_LOG_ACTORS; i++){
				BlockingQueue_add(Q.log_q, quit);
			}
			break;	
  		}else{
  			if(FD_ISSET(0, &fds)){
  				printf("Keyboard Input Detected!\n");
  				c = 0;
  				while ((r = getchar()) != '\n' && r != EOF) {
  					if(c == 0 && ( r == 'q' || r == 'Q' )){
  						c++;
  					}
  					else if(c == 1 && ( r == 'u' || r == 'U' )){
  						c++;
  					}
  					else if(c == 2 && ( r == 'i' || r == 'I' )){
  						c++;
  					}
  					else if(c == 3 && ( r == 't' || r == 'T' )){
  						c = -1;
  					}
  				}
  				if(c ==  -1){
  					printf("Quit Command Received!\n");
  					
					for(i = 0; i < NUM_HANDLE_CLIENT_ACTORS; i++){
  						BlockingQueue_add(Q.handle_client_q, quit);
					}
					
					for(i = 0; i < NUM_LOG_ACTORS; i++){
  						BlockingQueue_add(Q.log_q, quit);
					}
  					break;
				}
  			}
  			
  			if(FD_ISSET(sockfd, &fds)){
  				remfd = accept(sockfd, (struct sockaddr*)&rem_addr, &rem_addr_len);
				if(remfd < 0){
					printf("Error accepting socket: %s\n", strerror(errno));
					continue;
				}else{
					connptr = malloc(sizeof(struct connection_data));
					((struct connection_data*)connptr)->fd = remfd;
					((struct connection_data*)connptr)->addr = rem_addr;
					((struct connection_data*)connptr)->addr_len = rem_addr_len;
					BlockingQueue_add(Q.handle_client_q, connptr);
				}
  			}
  		}
  	}
 
	for(i = 0; i < NUM_HANDLE_CLIENT_ACTORS; i++){
		pthread_join(handle_client_threads[i], NULL);
	}
	
	pthread_join(log_thread, NULL);
	
	pthread_join(file_thread, NULL);
	
	printf("\nAll Threads Terminated...");
  	
	printf("Shutting Down Server...\n"); 	
	err = shutdown(sockfd, 2);
	if(err < 0){
    	printf("%s\n", strerror(errno));
	}
	
	printf("Done.\n");
}
