//Name: Robert Appelmann
#include "HandleClientActor.h"
#include "FileActor.h"
#include "BlockingQueue.h"

extern char* quit;

char* chat_file = "../resources/chat.txt";

void* handle_client_actor(void *arg){
	//Local Variables	
	//file descriptors for sockets objects
  	int remfd; 
 	int err;
  	
  	//I/O Message Stuff
	char mess_buff[8192];
	char submit_buff[1024];
	char rec_buff[8192];
 	int mess_len = 0;

	//Random Stuff
	int c, i, total_sent;
	char* cptr_start;
	void* conn_ptr;
	
	//File stuff
	FILE *chatFile;
		
	//Logging Stuff
	char log_buffer[1024];

	ActorQueues* Q = arg;
	BlockingQueue* return_queue = BlockingQueue_create();
	
	struct file_return* response;
	
	struct file_actor_request far;
	
	struct connection_data* client;
	struct http_request request;
	
	far.file_req = &request;
	far.return_queue = return_queue;
	
	while(1){
		conn_ptr = BlockingQueue_remove(Q->handle_client_q);
		
		if(((char*)conn_ptr)[0] == 'Q' && ((char*)conn_ptr)[1] == 'U' && ((char*)conn_ptr)[2] == 'I' && ((char*)conn_ptr)[3] == 'T'){
			//Quitting Time!!!
			break;
		}
		
		client = (struct connection_data*)conn_ptr;

		remfd = client->fd;

		sprintf(log_buffer, "HCA: Client <%s> Connected...", inet_ntoa(client->addr.sin_addr));

		mess_len = recv(remfd, rec_buff, 8192, 0);

		if(mess_len < 0){
			sprintf(log_buffer, "%s%s\n", log_buffer, strerror(errno));
			free(conn_ptr);
			close(remfd);
			continue;
		}
		
		err = process_request(rec_buff, &request, mess_len);
		if(err < 0){
			sprintf(log_buffer, "%sError processing Request. May not have been valid HTTP\n", log_buffer);
			free(conn_ptr);
			close(remfd);
			continue;	
		}
		
		if(request.method == GET){
			//printf("Entering GET\n");
			sprintf(log_buffer, "%sGET <%s>...", log_buffer, request.path);
			
			if(!strcmp(request.path, "/")){
				sprintf(mess_buff, "HTTP/1.0 200 OK\r\n\r\n<html><meta http-equiv =\"refresh\" content=\"0; url=/pages/basic.html\"/></html>\r\n\0");
				err = send(remfd, mess_buff, strlen((char*)mess_buff), 0);
				if(err < 0){
					sprintf(log_buffer, "%s%s\n", log_buffer, strerror(errno));
					free(conn_ptr);
					continue;
				}
				sprintf(log_buffer, "%sRedirected To <pages/basic.html>...", log_buffer);
			}else{
				BlockingQueue_add(Q->file_q, (void*)&far);
											
				while(1){					
					response = BlockingQueue_remove(far.return_queue);
					if(!strcmp(response->data, "DONE")){
						break;
					}
					total_sent = 0;
					while(response->size > 0){
						err = send(remfd, response->data, response->size, 0);
						if(err < 0){
							printf("Error! %s\n", strerror(errno));
							break;
						}
						total_sent += err;
						response->size -= err;
					}
					free(response->data);
					free(response);
				}
				
			//printf("Exiting GET\n");
			}
		}else if(request.method == POST){	
			//printf("Entering POST\n");
			c = 0;
			while(request.body[c] != '='){
				c++;
			}
			c++;
			i=0;
			while(request.body[c+i]){
				submit_buff[i] = request.body[c+i];
				i++;
			}
			submit_buff[i-1] = '\0';
			
			chatFile = fopen(chat_file, "a");
				
			if(chatFile){
				fprintf(chatFile, "%s>: %s\n", inet_ntoa(client->addr.sin_addr), submit_buff);
				fclose(chatFile);
			}	
		}
				
		bzero(&mess_buff, 8192);
		bzero(&rec_buff, 8192);
		close(remfd);

		sprintf(log_buffer, "%sClient Closed\n\0", log_buffer);
		
		int log_len = strlen(log_buffer);
		char* log_message = malloc(log_len);
		if(log_message){
			memcpy(log_message, log_buffer, log_len);
			BlockingQueue_add(Q->log_q, log_message);
		}
		
		free(conn_ptr);
		free(request.body);
		free(request.path);
	}
	printf("Handle Client Thread Done!\n");
	return NULL;
}

int process_request(char* req, struct http_request* request, int length){
	int i, j, c;
	char* cptr_start;
	
	//printf("Entering process request\nGetting Method");
	
	//Get request method
	if(req[0] == 'G' && req[1] == 'E' && req[2] == 'T' && req[3] == ' '){
		request->method = GET;
	}else if(req[0] == 'P' && req[1] == 'O' && req[2] == 'S' && req[3] == 'T' && req[4] == ' '){
		request->method = POST;
	}else if(req[0] == 'H' && req[1] == 'E' && req[2] == 'A' && req[3] == 'D' && req[4] == ' '){
		request->method = HEAD;
	}else{
		return -ENOMETHOD;
	}
	
	//Get Path
	//determine what file they want
	//printf("Getting Path\n");
	
	cptr_start = strchr(req, '/');
	if(!cptr_start){
		return -ENOPATH;
	}
	i = 0;
	while(cptr_start[i] != ' ' && cptr_start[i] != '?' ){
		i++;
	}
	request->path = (char*)malloc(i+1);
	for(j = 0; j < i; j++){
		request->path[j] = cptr_start[j];
	}
	request->path[j] = '\0';	
	
	//Get Path-Params
	/*
	printf("Getting Path Params")
	
	cptr_start = strchr(req, '?');
	if(cptr_start){
		while(1){
			//has params
			i = 0;
			while((cptr_start[i+1] >= '0' && cptr_start[i+1] <= '9') || (cptr_start[i+1] >= 'a' && cptr_start[i+1] <= 'z') || (cptr_start[i+1] >= 'A' && cptr_start[i+1] <= 'Z')){
				i++;
			}
		}
	}
	*/
	
	//Get HTTP Version
	//Get Headers
	//Get Body
	//printf("Getting Body\n");
	
	c = 0;
	cptr_start = &req[4];
	while(!(cptr_start[c-1] == '\n' && cptr_start[c-2] == '\r' && cptr_start[c-3] == '\n' && cptr_start[c-4] == '\r') && c < length){
		c++;
	}
	cptr_start = &cptr_start[c];
	
	c = 0;
	while(cptr_start[c] != '/0' && c < length){
		c++;
	}
	
	request->body = malloc(c+1);
	memcpy(request->body, cptr_start, c);
	request->body[c] = '\0';
	
	//printf("Leaving process request\n");
	
	return 0;
}