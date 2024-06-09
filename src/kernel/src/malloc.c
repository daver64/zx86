#include <system.h>
#include <bget.h>
extern uint32_t heap_end;
extern uint32_t heap_start;
extern uint32_t hmem;
extern uint32_t user_elf_start;
extern uint32_t user_elf_end;
extern uint32_t unspecified_start;
extern uint32_t unspecified_end;
void mem_init()
{
	int stacksize=1024*1024*4;
	int unspecifiedsize=1024*1024*256;
	heap_end = (hmem * 1024) - stacksize - unspecifiedsize; // top of memory - 4Mb to allow for stack
	unspecified_start=heap_end;
	unspecified_end=(hmem*1024) - stacksize;
	heap_start = 0x6000000;						  // heap starts at 100mb and extends to the top of memory - 4Mb
	bpool((void *)heap_start, (bufsize)(heap_end - heap_start));
	user_elf_start = 0x2000000;
	user_elf_end = 0x6000000;
}

void *malloc(size_t size)
{
	return bget(size);
}

void *calloc(size_t count, size_t size)
{
	return bgetz(count * size);
}

void *realloc(void *ptr, size_t size)
{
	return bgetr(ptr, size);
}

void free(void *ptr)
{
	brel(ptr);
}

void *aligned_malloc(size_t size, int alignment)
{
	void *p1;
	void **p2;
	int offset = alignment - 1 + sizeof(void *);
	// printf("aligned malloc: offset=%u totsize=%u\n",offset,size+offset);
	p1 = malloc(size + offset);
	if (!p1)
		return NULL;
	p2 = (void **)(((uintptr_t)(p1) + offset) & ~(alignment - 1));
	p2[-1] = p1;
	return p2;
}

void aligned_free(void *ptr)
{
	free(((void **)ptr)[-1]);
}

void print_mem_stats()
{
	bufsize curalloc, totfree, maxfree;
	long nget, nrel;
	bstats(&curalloc, &totfree, &maxfree, &nget, &nrel);
	printf("allocated=%u\nfree=%u\nmaxfree=%u\n", curalloc, totfree, maxfree);
}

void print_heap_stats()
{
	bufsize curalloc, totfree, maxfree;
	long nget, nrel;
	bstats(&curalloc, &totfree, &maxfree, &nget, &nrel);
	float ftmp = (float)maxfree;
	ftmp /= (1024.0 * 1024.0);
	printf("Heap Size=%0.0fM\n", ftmp);
}
