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

#define NUMTHREADS 4
#define PORT1 12340