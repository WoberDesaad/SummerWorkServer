#ifndef _HANDLE_CLIENT_ACTOR_H_
#define _HANDLE_CLIENT_ACTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>   
#include <time.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "Actor.h"
#include "BlockingQueue.h"
#include "FileActor.h"

#define FILE_BUFFER_SIZE 8192

struct connection_data{
	int fd;
	struct sockaddr_in addr;
	unsigned int addr_len;
};

//http methods
#define POST 1
#define GET 2
#define HEAD 3

//http version
#define HTTP10 1
#define HTTP11 2

#define ENOMETHOD 4
#define ENOPATH 3

struct http_header{
	char* key;
	char* value;
};

struct request_params{
	char* key;
	char* value;
};

struct http_request{
	int method;
	char* path;
	struct request_params* params;
	int version;
	struct http_header* headers;
	char* body;
};

void* handle_client_actor(void *arg);
int process_request(char* req, struct http_request* request);
int send_all(int sock_fd, char* buffer, int n);

#endif
