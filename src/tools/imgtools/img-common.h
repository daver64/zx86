#ifndef _IMG_COMMON_H_
#define _IMG_COMMON_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<string.h>
#include"ff.h"

extern FILE* ffp;
extern FATFS* rootfs;
int hdc_read_sectors(int32_t lba, int32_t count, void* buffer);
int hdc_write_sectors(int32_t lba,int32_t count, void* buffer);
unsigned int hdc_mount();
void hdc_unmount();
FRESULT scan_files(char* path);



#endif

