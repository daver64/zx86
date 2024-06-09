
#include"img-common.h"

PARTITION VolToPart[FF_VOLUMES] = {
	{0, 1}
};

FILE* ffp=NULL;
FATFS* rootfs=NULL;

FRESULT scan_files(char* path);
unsigned int hdc_mount();

int hdc_read_sectors(int32_t lba, int32_t count, void* buffer)
{
	fseek(ffp,lba*512,SEEK_SET);
	fread(buffer,1,count*512,ffp);
	return 0;
}
int hdc_write_sectors(int32_t lba,int32_t count, void* buffer)
{
	fseek(ffp,lba*512,SEEK_SET);
	fwrite(buffer,1,count*512,ffp);
	return 0;
}
void hdc_unmount()
{
	f_mount(0,"",0);
}
unsigned int hdc_mount()
{
	rootfs=malloc(sizeof(FATFS));
	FRESULT r=f_mount(rootfs,"0:",1);
	if(r!=0)
	{
		printf("dfs mount error %d\n",r);
		free(rootfs);
		rootfs=NULL;
		return r;
	}
	DWORD fre_clust;//,fre_sect,tot_sect;
	
	char str[12];
	memset(&str[0],0,12);
	f_getlabel("0:",str,0);
	int i;
	printf("Volume :");

	for(i=0;i<12;i++)
	{
		putchar(str[i]);
	}
	putchar('\n');

	r=f_getfree("",&fre_clust,&rootfs);
	DWORD tot_sect=(rootfs->n_fatent-2)*rootfs->csize;
	DWORD fre_sect=fre_clust*rootfs->csize;

	DWORD totdrivespace=tot_sect/2;
	DWORD freespace=fre_sect/2;
	DWORD totdiv=1;
	DWORD freediv=1;
	char tfmt[16];
	char ffmt[16];
	memset(tfmt,0,16);
	memset(ffmt,0,16);
	if(totdrivespace>1024*1024)
	{
		sprintf(tfmt,"%s","MB");
		totdiv=1024;
	}
	else if(totdrivespace>1024*1024*1024)
	{
		sprintf(tfmt,"%s","GB");
		totdiv=1024*1024;
	}
	else
	{
		sprintf(tfmt,"%s","B");
		totdiv=1;
	}

	if(freespace>1024*1024)
	{
		sprintf(ffmt,"%s","MB");
		freediv=1024;
	}
	else if(freespace>1024*1024*1024)
	{
		sprintf(ffmt,"%s","GB");
		freediv=1024*1024;
	}
	else
	{
		sprintf(ffmt,"%s","B");
		freediv=1;
	}
	printf("%10u %s total drive space.\n%10u %s available.\n", 
			totdrivespace/totdiv, &tfmt[0], freespace/freediv, &ffmt[0]);	
	return r;
}
FRESULT scan_files(char* path)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;

		//uint8_t ofgc=get_foreground_colour();
	//	set_foreground_colour(VGA_YELLOW);
    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) 
						{                    /* It is a directory */
							printf("[%s]\n",fno.fname);
                //i = strlen(path);
                //sprintf(&path[i], "/%s", fno.fname);
                //res = scan_files(path);                     
                //if (res != FR_OK) break;
                //path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%-30s\t%8u\n", fno.fname,fno.fsize);
            }
        }
        f_closedir(&dir);
    }
	//	set_foreground_colour(ofgc);
    return res;
}

