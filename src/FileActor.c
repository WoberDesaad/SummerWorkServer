#include "FileActor.h"

void* file_actor(void* arg){
	while(1){
		
	}
}

				sendFile = fopen(cptr_start, "r");
				if(sendFile == NULL){
					sprintf(mess_buff, "HTTP/1.0 404 File Not Found\r\n\r\n<html><head>ERROR 404... Page Not Found!!!</head></html>\r\n");
					sprintf(log_buffer, "%sFile Not Found...", log_buffer);

					err = send(remfd, mess_buff, strlen((char*)mess_buff), 0);
					if(err < 0){
						sprintf(log_buffer, "%s%s\n", log_buffer, strerror(errno));
						free(conn_ptr);
						continue;
					}	
				}else{
					sprintf(mess_buff, "HTTP/1.0 200 OK\r\n\r\n");
					sprintf(log_buffer, "%sFile Found..", log_buffer);
					
					err = send(remfd, mess_buff, strlen((char*)mess_buff), 0);
					if(err < 0){
						sprintf(log_buffer, "%s%s\n", log_buffer, strerror(errno));
						free(conn_ptr);
						continue;
					}	
					
					while( !feof(sendFile) ){
						int numread = fread(mess_buff, sizeof(unsigned char), 1000, sendFile);
						if( numread < 1 ) break; // EOF or error
					
						char *mess_buffer_ptr = mess_buff;
						do{
							int numsent = send(remfd, mess_buffer_ptr, numread, 0);
							if( numsent < 1 ){ // 0 if disconnected, otherwise error
								break; // timeout or error
							}
							mess_buffer_ptr += numsent;
							numread -= numsent;
						}while(numread > 0);
					}
					fclose(sendFile);
					
					sprintf(log_buffer, "%sSent...", log_buffer);
				}
				bzero(&str_buff, 256);