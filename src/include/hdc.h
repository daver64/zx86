#ifndef _HDC_H_
#define _HDC_H_

#ifdef WIN32
#define DFS_HOSTED
#include <windows.h>
#endif

#ifndef DFS_HOSTED
#include <system.h>
#endif



//
// these are hard coded to a particular disc / disc image size
// in this case the accompanying 260MB file
//

#define BLOCK_MAP_SIZE (512)
#define BLOCK_BUFFER_SIZE (8192)
#define BLOCK_DATA_SIZE (BLOCK_BUFFER_SIZE - (1*sizeof(int32_t)))
#define SECTOR_SIZE (512)
#define SECTORS_PER_BLOCK (BLOCK_BUFFER_SIZE/SECTOR_SIZE)
#define MAX_DIR_ENTRIES (64)

#define MAX_LBA (520000)
#define MIN_LBA (0)
#define MAX_FILENAME (24)

typedef struct HDC_DIR_ENTRY
{
	char name[MAX_FILENAME];
	int32_t block;
	int32_t length;
} HDC_DIR_ENTRY;

typedef struct HDC_BLOCK
{
	unsigned char data[BLOCK_DATA_SIZE];
	int32_t nextblock;
} HDC_BLOCK;

typedef struct FILE
{
	HDC_DIR_ENTRY *dirent;
	int32_t position;
	unsigned char *buffer;
	uint32_t buffersize;
	char mode;
} FILE;

/* HDC.C */
void hdc_dump_filehandle(FILE *fp);
int32_t hdc_format();
int32_t hdc_init();
int32_t hdc_sync();
void hdc_print_directorymap();
void hdc_print_blockmap(int32_t numblocks);
int32_t hdc_allocate_block();
void hdc_release_block(int32_t blockid);
int32_t hdc_append_block(int32_t blockid);
int32_t hdc_extend_file(const char *name);
int32_t hdc_trim_file(const char *name);
void hdc_release_dirent(const char *name);
int32_t hdc_allocate_dirent_id(const char *name);
int32_t hdc_does_dirent_exist(const char *name);
int32_t hdc_count_blocks(const char *name);
int32_t hdc_get_capacity(const char *name);
int32_t hdc_get_dirent_id(const char *name);
int32_t hdc_read_block(int32_t blockid,unsigned char *buffer);
int32_t hdc_write_block(int32_t blockid,unsigned char *buffer);
HDC_DIR_ENTRY *hdc_get_dirent(const char *name);
int32_t hdc_get_filesize(const char *name);
int32_t hdc_set_filesize(const char *name,int32_t length);
int32_t hdc_write_file(const char *name, void *buffer, int32_t bufferlen);
int32_t hdc_read_file(const char *name, void *buffer, int32_t bufferlen);
void test_disc();

FILE *hdc_fopen(const char *filename,const char *mode);
int32_t hdc_fread(void *buffer,int size,int count,FILE *fp);
int32_t hdc_fwrite(void *buffer,int size, int count, FILE *fp);
int32_t hdc_fseek(FILE *fp, int32_t offset, int32_t origin);
int32_t hdc_ftell(FILE *fp);
int32_t hdc_feof(FILE *fp);
int32_t hdc_rewind(FILE *fp);
int32_t hdc_fgetc(FILE *fp);
int32_t hdc_getc(FILE *fp);

#endif

