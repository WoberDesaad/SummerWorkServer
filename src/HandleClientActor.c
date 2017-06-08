//Name: Robert Appelmann
#include "HandleClientActor.h"
#include "FileActor.h"
#include "BlockingQueue.h"

extern char* quit;

char* chat_file = "chat.txt";

void* handle_client_actor(void *arg){
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
	
	//File stuff
	FILE *sendFile;
	FILE *chatFile;
		
	//Logging Stuff
	char log_buffer[1024];
	int log_len;

	ActorQueues* Q = arg;
	BlockingQueue* return_queue = BlockingQueue_create();
	
	struct file_return* response;
	
	char file_buffer[FILE_BUFFER_SIZE];
	
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
			printf("FREE1\n");
			free(conn_ptr);
			continue;
		}
		
		err = process_request(rec_buff, &request);
	
		if(request.method == GET){
			printf("Get %s\n", request.path);
			sprintf(log_buffer, "%sGET </%s>...", log_buffer, request.path);
			
			if(!strcmp(request.path, " ")){
				sprintf(mess_buff, "HTTP/1.0 200 OK\r\n\r\n<html><meta http-equiv =\"refresh\" content=\"0; url=/pages/basic.html\"/></html>\r\n");
				err = send(remfd, mess_buff, strlen((char*)mess_buff), 0);
				if(err < 0){
					sprintf(log_buffer, "%s%s\n", log_buffer, strerror(errno));
					printf("Free2\n");
					free(conn_ptr);
					continue;
				}
				printf("Redirected\n");
				sprintf(log_buffer, "%sRedirected To <pages/basic.html>...", log_buffer);
			}else{
				BlockingQueue_add(Q->file_q, (void*)&far);
											
				while(1){
					printf("Doing Stuff\n");
					
					response = BlockingQueue_remove(far.return_queue);
					
					printf("Response: %s\n", response->data);
					if(!strcmp(response->data, "DONE")){
						break;
					}
					while(response->size > 0){
						err = send(remfd, response->data, response->size, 0);
						if(err < 0){
							printf("Error!");
							break;
						}
						response->size -= err;
					}
				}
			}
		}else if(request.method == POST){
			/*
			 * c = 0;
			while(c < mess_len - 4 && !( rec_buff[c] == '\r' && rec_buff[c+1] == '\n' && rec_buff[c+2] == '\r' && rec_buff[c+3] == '\n')){
				c++;
			}
			while(rec_buff[c] != '='){
				c++;
			}
			
			cptr_start = &rec_buff[c+1];
			c = 0;
			while(cptr_start[c] != '&'){
				if(cptr_start[c] == '+'){
					submit_buff[c] = ' ';
				}else if(cptr_start[c] == '%'){
					submit_buff[c] = ' ';
					submit_buff[c+1] = ' ';
					submit_buff[c+2] = ' ';
					c+=2;
				}else{
					submit_buff[c] = cptr_start[c];
				}
				c++;
			}
			submit_buff[c] = '\0';
			
			chatFile = fopen(chat_file, "a");
				
			fprintf(chatFile, "%s>: %s\n", inet_ntoa(client->addr.sin_addr), submit_buff);
			
			if(!chatFile){
				printf("Error Opening File!\n");
				continue;
			}
			
			fclose(chatFile);
			
			
			sprintf(mess_buff, "HTTP/1.0 200 OK\r\n\r\n<html><meta http-equiv =\"refresh\" content=\"0; url=/pages/chat.html\"/></html>\r\n");
			err = send(remfd, mess_buff, strlen((char*)mess_buff), 0);
			if(err < 0){
				sprintf(log_buffer, "%s%s\n", log_buffer, strerror(errno));
				free(conn_ptr);
				continue;
			}
			sprintf(log_buffer, "%sRefreshing <chat.html>...", log_buffer);
				*/
		}else if(request.method == HEAD){
			
		}else{
			
		}
				
		bzero(&mess_buff, 8192);
		bzero(&rec_buff, 8192);
		close(remfd);

		sprintf(log_buffer, "%sClient Closed\n\0", log_buffer);
		log_len = strlen(log_buffer);
		log_message = malloc(log_len);
		if(log_message){
			memcpy(log_message, log_buffer, log_len);
			BlockingQueue_add(Q->log_q, log_message);
		}

		bzero(&log_buffer, 1024);
		
		printf("Freeing!\n");
		
		free(conn_ptr);
	}
	
	printf("Handle Client Thread Done!\n");
	return NULL;
}

int process_request(char* req, struct http_request* request){
	int i, j;
	char* cptr_start;
	
	//Get request metod
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
	cptr_start = strchr(req, '/');
	if(!cptr_start){
		return -ENOPATH;
	}
	i = 0;
	while(cptr_start[i+1] != ' ' && cptr_start[i+1] != '?' ){
		i++;
	}
	request->path = (char*)malloc(i+1);
	for(j = 0; j <= i; j++){
		request->path[j] = cptr_start[j+1];
	}
	request->path[j] = '\0';	
	
	//Get Path-Params
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
	
	//Get HTTP Version
	//Get Headers
	//Get Body
	return 0;
}

int send_all(int sock_fd, char* buffer, int n){
	int err, sent;
	while(sent < n){
		err = send(sock_fd, &buffer[sent], n, 0);
		if(err < 0){
			return err;
		}
		sent += err;
	}
	return sent;
}
