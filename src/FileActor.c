#include "FileActor.h"

char* done = "DONE";

void* file_actor(void* arg){
	struct file_actor_request* far;
	ActorQueues* Q = (ActorQueues*)arg;
	
	char log_buffer[1024];
	char tmp_buffer[8192];
	
	int ret_len;
	
  	char dir[256] = "../resources";
  	
  	//getcwd(dir, 256);
  	
  	int dir_start = strlen(dir);
	
	FILE* sendFile = NULL;
	
	while(1){
		far = BlockingQueue_remove(Q->file_q);
	
		if(((char*)far)[0] == 'Q' && ((char*)far)[1] == 'U' && ((char*)far)[2] == 'I' && ((char*)far)[3] == 'T'){
			//quitting time
			break;
		}
	
		memcpy(&dir[dir_start], far->file_req->path, strlen(far->file_req->path));
		dir[dir_start + strlen(far->file_req->path)] = '\0';
		
		sendFile = fopen(dir, "r");
		if(sendFile == NULL){
			sprintf(tmp_buffer, "HTTP/1.0 404 File Not Found\r\n\r\n<html><body>ERROR 404... Resource %s not found!!!</body></html>\r\n\0", dir);
			sprintf(log_buffer, "File Actor: File <%s> Not Found...Sending 404\0", dir);
			
			return_data(far->return_queue, tmp_buffer, strlen(tmp_buffer)+1);
			return_data(far->return_queue, done, strlen(done)+1);			
		}else{
			
			sprintf(tmp_buffer, "HTTP/1.0 200 OK\r\n");
			//add headers
			
			sprintf(tmp_buffer, "%s\r\n", tmp_buffer);//blank line that separates headers from body
			
			sprintf(log_buffer, "File Actor: File %s found..", far->file_req->path);
			
			return_data(far->return_queue, tmp_buffer, strlen(tmp_buffer));
			bzero(tmp_buffer, strlen(tmp_buffer));	
			
			while( !feof(sendFile) ){
				ret_len = fread(tmp_buffer, sizeof(unsigned char), 8192, sendFile);
				if( ret_len < 1 ) break; // EOF or error
				return_data(far->return_queue, tmp_buffer, ret_len);
				bzero(tmp_buffer, ret_len);
			}
			fclose(sendFile);
			
			return_data(far->return_queue, done, strlen(done)+1);
			
			sprintf(log_buffer, "%sSent\n\0", log_buffer);
		}
		
		int log_len = strlen(log_buffer);
		char* log_message = malloc(log_len);
		if(log_message){
			memcpy(log_message, log_buffer, log_len);
			BlockingQueue_add(Q->log_q, log_message);
		}
	}
	return NULL;
}

void return_data(BlockingQueue* Q, char* data, int ret_len){	
	struct file_return* ret_file = (struct file_return*)malloc(sizeof(struct file_return));
	
	ret_file->size = ret_len;
	ret_file->data = (char*)malloc(ret_len);
	
	memcpy(ret_file->data, data, ret_len);
	
	BlockingQueue_add(Q, ret_file);
}
