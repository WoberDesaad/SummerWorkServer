#include <stdlib.h>
#include <string.h>

char* ptr_cpy(char* buffer, int len){
	char* str_ptr = (char*)malloc(len);
	if(!str_ptr){
		return NULL;
	}
	memcpy(str_ptr, buffer, len);
	return str_ptr;
}