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

#define NUM_HANDLE_CLIENT_ACTORS 1
#define NUM_LOG_ACTORS 1
#define PORT1 12340
