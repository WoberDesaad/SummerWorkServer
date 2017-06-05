#include "FileActor.h"

char* done = "DONE";

void* file_actor(void* arg){
	struct file_actor_request* far;
	struct file_request* resource;
	ActorQueues* Q = (ActorQueues*)arg;
	
	char log_buffer[1024];
	char tmp_buffer[8192];
	char* ret_file;
	
	int ret_len;
	
	FILE* sendFile = NULL;
	
	while(1){
		far = BlockingQueue_remove(Q->file_actor_q);
		
		if(((char*)far)[0] == 'Q' && ((char*)far)[1] == 'U' && ((char*)far)[2] == 'I' && ((char*)far)[3] == 'T'){
			//quitting time
			return NULL;
		}
		
		sendFile = fopen(far->file_req->file_name, "r");
		if(sendFile == NULL){
			sprintf(tmp_buffer, "HTTP/1.0 404 File Not Found\r\n\r\n<html><body>ERROR 404... Resource %s not found!!!</body></html>\r\n", far->file_req->file_name);
			sprintf(log_buffer, "File Actor: File <%s> Not Found...Sending 404\0", far->file_req->file_name);
			
			BlockingQueue_add(Q->log_q, log_buffer);
			
			return_data(far->return_queue, tmp_buffer, strlen(tmp_buffer));
			BlockingQueue_add(far->return_queue, done);
			
			far = NULL;
			//zero out buffers
			bzero(tmp_buffer, strlen(tmp_buffer));
			
			continue;
		}else{
			sprintf(tmp_buffer, "HTTP/1.0 200 OK\r\n");
			//add headers
			
			sprintf(tmp_buffer, "%s\r\n", tmp_buffer);//blank line that separates headers from body
			
			sprintf(log_buffer, "File Actor: File %s found..", far->file_req->file_name);
			
			return_data(far->return_queue, tmp_buffer, strlen(tmp_buffer));
			bzero(tmp_buffer, strlen(tmp_buffer));	
			
			while( !feof(sendFile) ){
				int ret_len = fread(tmp_buffer, sizeof(unsigned char), 8192, sendFile);
				if( ret_len < 1 ) break; // EOF or error
				return_data(far->return_queue, tmp_buffer, ret_len);
				bzero(tmp_buffer, num_read);
			}
			fclose(sendFile);
			
			BlockingQueue_add(far->return_queue, done);
			
			sprintf(log_buffer, "%sSent...", log_buffer);
		}
	}
}

void return_data(BlockingQueue* Q, char* data, int ret_len){	
	struct file_return* ret_file = (struct file_return*)malloc(sizeof(struct file_return));
	
	ret_file->len = ret_len;
	ret_file->data = (char*)malloc(ret_len);
	
	memcpy(ret_file->data, data, ret_len);
	
	BlockingQueue_add(Q, ret_file);
}
