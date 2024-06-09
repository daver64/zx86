#include <system.h>


int create_thread(void *code, size_t stacksize)
{
	unsigned char *cp=(unsigned char*)code;
	int thread_id = initialise_thread(cp,stacksize);
    return thread_id;
}