#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ff.h>

FILE *stdin = (void *)0;
FILE *stdout = (void *)1;
FILE *stderr = (void *)2;

FATFS *rootfs = NULL;
FATFS *ramfs = NULL;
FRESULT scan_files(char *path);
FRESULT scan_folders(char *path);
uint32_t hdc_get_primary_sector_count();
PARTITION VolToPart[FF_VOLUMES] = {
	{0, 1}};

unsigned int fgetsize(FILE *fp);
void dfs_report_space();

int dfs_fdisc(int argc, char **argv)
{
	uint32_t pls = hdc_get_primary_sector_count();
	if (argc == 2)
	{
		pls = strtoul(argv[1], NULL, 16);
	}
	dfs_unmount();
	BYTE work[FF_MAX_SS];
	LBA_t plist[] = {pls};
	f_fdisk(0, plist, work);
	f_mkfs("0:", 0, work, sizeof work);
	dfs_mount();
	int result = 0;
	return result;
}

unsigned int dfs_mount_hdc()
{
	rootfs = malloc(sizeof(FATFS));
	FRESULT r = f_mount(rootfs, "0:", 1);
	if (r != 0)
	{
		printf("dfs mount error %d\n", r);
		free(rootfs);
		rootfs = NULL;
		return r;
	}

	char str[12];
	memset(&str[0], 0, 12);
	f_getlabel("0:", str, 0);
	int i;
	printf("Volume :");

	for (i = 0; i < 12; i++)
	{
		putchar(str[i]);
	}
	putchar('\n');
	DWORD fre_clust, fre_sect, tot_sect;

	r = f_getfree("", &fre_clust, &rootfs);
	tot_sect = (rootfs->n_fatent - 2) * rootfs->csize;
	fre_sect = fre_clust * rootfs->csize;
	printf("%10lu Mb total drive space.\n%10lu Mb available.\n", ((tot_sect / 2) / 1024), ((fre_sect / 2) / 1024));
	return r;
}
void dfs_report_space()
{
	DWORD fre_clust, fre_sect, tot_sect;
	FRESULT r = f_getfree("", &fre_clust, &rootfs);
	tot_sect = (rootfs->n_fatent - 2) * rootfs->csize;
	fre_sect = fre_clust * rootfs->csize;
	printf("%u Mb total drive space.\n%lu Mb available.\n", ((tot_sect / 2) / 1024), ((fre_sect / 2) / 1024));
}

unsigned int dfs_mount_rdc()
{
	BYTE work[FF_MAX_SS];

	ramfs = malloc(sizeof(FATFS));
	FRESULT r = f_mount(ramfs, "2", 1);
	if (r != 0)
	{
		printf("rfs mount error %d\n", r);
		free(ramfs);
		rootfs = NULL;
		return r;
	}
	DWORD fre_clust, fre_sect, tot_sect;
	f_mkfs("2", 0, work, sizeof work);
	char str[12];
	memset(&str[0], 0, 12);
	f_getlabel("2:", str, 0);
	int i;
	printf("Volume :");

	for (i = 0; i < 12; i++)
	{
		putchar(str[i]);
	}
	putchar('\n');

	r = f_getfree("", &fre_clust, &ramfs);
	tot_sect = (ramfs->n_fatent - 2) * ramfs->csize;
	fre_sect = fre_clust * ramfs->csize;
	printf("%10lu Mb total drive space.\n%10lu Mb available.\n", ((tot_sect / 2) / 1024), ((fre_sect / 2) / 1024));
	return r;
}
unsigned int dfs_mount()
{
	dfs_mount_hdc();
	// dfs_mount_rdc();
}
unsigned int dfs_unmount()
{
	if (!rootfs)
		return 0;
	FRESULT r = f_mount(0, "", 0);
	free(rootfs);
	rootfs = NULL;
	return r;
}
int dfs_mkfs(int argc, char **argv)
{
	BYTE work[FF_MAX_SS];
	f_mkfs("", 0, work, sizeof work);
	int result = 0;
	return result;
}
int dfs_cp(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("usage:\ncp <sourcefile> <destinationfile>\n");
		return 1;
	}
	FILE *fps = fopen(argv[1], "r");
	if (fps)
	{
		unsigned int len = fgetsize(fps);
		unsigned char *buffer = malloc(len);
		fread(buffer, 1, len, fps);
		fclose(fps);
		fps = NULL;
		fps = fopen(argv[2], "w");
		if (fps)
		{
			fwrite(buffer, 1, len, fps);
			fclose(fps);
			free(buffer);
		}
		else
		{
			printf("could not open destination file %s\n", argv[2]);
			free(buffer);
			return 2;
		}
	}
	else
	{
		printf("could not open source file %s\n", argv[1]);
		return 1;
	}
	return 0;
}
int dfs_mv(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("usage:\nmv <sourcefile> <destinationfile>\n");
		return 1;
	}
	FILE *fps = fopen(argv[1], "r");
	if (fps)
	{
		unsigned int len = fgetsize(fps);
		unsigned char *buffer = malloc(len);
		fread(buffer, 1, len, fps);
		fclose(fps);
		fps = NULL;
		fps = fopen(argv[2], "w");
		if (fps)
		{
			fwrite(buffer, 1, len, fps);
			fclose(fps);
			free(buffer);
			f_unlink(argv[1]);
		}
		else
		{
			printf("could not open destination file %s\n", argv[2]);
			free(buffer);
			return 2;
		}
	}
	else
	{
		printf("could not open source file %s\n", argv[1]);
		return 1;
	}
	return 0;
}
int dfs_rm(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage:\nrm <filename>\n");
		return 1;
	}
	FRESULT r = f_unlink(argv[1]);
	return r;
}
int unlink(const char *filename)
{
	FRESULT r = f_unlink(filename);
	return r;
}
int dfs_ls(int argc, char **argv)
{
	char buff[256];
	if (argc == 1)
		f_getcwd(&buff[0], 128);
	else if (argc == 2)
		sprintf(buff, "%s", argv[1]);
	else
		return 0;
	scan_folders(buff);
	memset(buff, 0, 256);
	if (argc == 1)
		f_getcwd(&buff[0], 128);
	else if (argc == 2)
		sprintf(buff, "%s", argv[1]);
	else
		return 0;
	scan_files(buff);
	int result = 0;
	return result;
}

int dfs_mkdir(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage:\nmkdir <dirname>\n");
		return 1;
	}
	int result = 0;
	FRESULT r = f_mkdir(argv[1]);
	if (r != FR_OK)
	{
		printf("error creating directory %s\n", argv[1]);
	}
	return result;
}
int dfs_rmdir(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage:\nrmdir <dirname>\n");
		return 1;
	}
	int result = 0;
	FRESULT r = f_unlink(argv[1]);
	if (r != FR_OK)
	{
		printf("can't remove directory %s\n", argv[1]);
	}
	return result;
}
int dfs_cd(int argc, char **argv)
{
	if (argc == 0)
	{
		char buf[256];
		memset(buf, 0, 256);
		f_getcwd(&buf[0], 128);
		printf("%s\n", buf);
		return 0;
	}
	if (argc != 2 && argc != 0)
	{
		printf("usage:\ncd <dirname>\n");
		return 1;
	}
	int result = 0;
	FRESULT r = f_chdir(argv[1]);
	if (r != FR_OK)
	{
	}
	return result;
}

FRESULT scan_folders(char *path)
{
	disable_interrupts();
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;

	uint8_t ofgc = get_foreground_colour();
	set_foreground_colour(VGA_LIGHTBLUE);
	res = f_opendir(&dir, path); /* Open the directory */
	if (res == FR_OK)
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (fno.fattrib & AM_DIR)
			{ 
				for (int i = 0; i < 12; i++)
					fno.fname[i] = tolower(fno.fname[i]);
				printf("%-30s\t%8s\n", fno.fname, "<dir>");
			}
		}
		f_closedir(&dir);
	}
	set_foreground_colour(ofgc);
	enable_interrupts();
	return res;
}
int is_executable_filename(char *filename)
{
	return (strstr(filename, ".elf") != NULL || strstr(filename, ".com") != NULL);
}

FRESULT scan_files(char *path)
{
	disable_interrupts();
	disable_cursor();
	// disable_blink();
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;

	uint8_t ofgc = get_foreground_colour();

	set_foreground_colour(VGA_LIGHTGREY);
	res = f_opendir(&dir, path); /* Open the directory */
	if (res == FR_OK)
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0)
				break; /* Break on error or end of dir */
			if (fno.fattrib & AM_DIR)
			{	/* It is a directory */
				//	printf("[%s]\n",fno.fname);
				// i = strlen(path);
				// sprintf(&path[i], "/%s", fno.fname);
				// res = scan_files(path);
				// if (res != FR_OK) break;
				// path[i] = 0;
			}
			else
			{ /* It is a file. */
				for (int i = 0; i < 12; i++)
					fno.fname[i] = tolower(fno.fname[i]);
				if (is_executable_filename(fno.fname))
				{
					set_foreground_colour(VGA_RED);
					printf("%-30s\t%8u\n", fno.fname, fno.fsize);
					set_foreground_colour(VGA_LIGHTGREY);
				}
				else
				{
					printf("%-30s\t%8u\n", fno.fname, fno.fsize);
				}
			}
		}
		f_closedir(&dir);
	}
	set_foreground_colour(ofgc);
	enable_cursor();
	enable_interrupts();
	printf("\n");

	// enable_blink();

	return res;
}
FILE *path_fopen(const char *filename, const char *mode)
{
	FILE *fp;
	int argc;
	char **args;
	char *pvar = getenv("PATH");
	if (!pvar)
		return NULL;
	argc = string_split(pvar, ';', &args);
	FILE *fpresult = NULL;
	for (int i = 0; i < argc; i++)
	{
		char buffer[MAX_PATH];
		snprintf((char *)&buffer[0], MAX_PATH, "%s\\%s", args[i], filename);
		fp = fopen((char *)&buffer[0], mode);
		if (fp)
		{
			fpresult = fp;
			i = argc;
		}
	}
	for (int i = 0; i < argc; i++)
	{
		free(*(args + i));
	}
	free(args);
	return fpresult;
}

FILE *fopen(const char *filename, const char *mode)
{
	FIL *fp = malloc(sizeof(FIL));
	if (!fp)
	{
		return NULL;
	}
	FRESULT r;
	int fileisvalid = false;
	if (!strcmp(mode, "r"))
	{
		r = f_open(fp, filename, FA_READ);
	}
	else if (!strcmp(mode, "r+"))
	{
		r = f_open(fp, filename, FA_READ | FA_WRITE);
	}
	else if (!strcmp(mode, "w"))
	{
		r = f_open(fp, filename, FA_WRITE | FA_CREATE_ALWAYS);
	}
	else if (!strcmp(mode, "w+"))
	{
		r = f_open(fp, filename, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	}
	else if (!strcmp(mode, "a"))
	{
		r = f_open(fp, filename, FA_OPEN_APPEND | FA_WRITE);
	}
	else if (!strcmp(mode, "a+"))
	{
		r = f_open(fp, filename, FA_OPEN_APPEND | FA_WRITE | FA_READ);
	}
	else if (!strcmp(mode, "wx"))
	{
		r = f_open(fp, filename, FA_CREATE_NEW | FA_WRITE);
	}
	else if (!strcmp(mode, "w+x"))
	{
		r = f_open(fp, filename, FA_CREATE_NEW | FA_WRITE | FA_READ);
	}
	if (r)
	{
		free(fp);
		fp = NULL;
	}
	return fp;
}

int fclose(FILE *fp)
{
	if (fp == stdin || fp == stdout || fp == stderr)
		return 0;
	FIL *ffp = (FIL *)fp;
	FRESULT r = f_close(ffp);
	free(ffp);
	return r;
}

unsigned int ftell(FILE *fp)
{
	if (fp == stdin || fp == stdout || fp == stderr)
		return 0;
	FIL *ffp = (FIL *)fp;
	FSIZE_t t = f_tell(ffp);
	return t;
}

unsigned int fread(void *buffer, unsigned int size, unsigned int count, FILE *fp)
{
	if (fp == stdin)
	{
		char *ptr = (char *)buffer;
		unsigned nchars = size * count;
		for (unsigned i = 0; i < nchars; i++)
		{
			ptr[i] = getchar();
			putchar(ptr[i]);
		}
		return nchars;
	}
	if (fp == stdout || fp == stderr)
	{
		return 0;
	}
	FIL *ffp = (FIL *)fp;
	UINT br;
	FRESULT r = f_read(ffp, buffer, count * size, &br);
	return br;
}

unsigned int fwrite(void *buffer, unsigned int size, unsigned int count, FILE *fp)
{
	if (fp == stdout || fp == stderr)
	{
		char *ptr = (char *)buffer;
		unsigned char nchars = size * count;
		for (unsigned i = 0; i < nchars; i++)
		{
			putchar(ptr[i]);
		}
	}
	if (fp == stdin)
	{
		return 0;
	}
	FIL *ffp = (FIL *)fp;
	UINT bw;
	FRESULT r = f_write(ffp, buffer, count * size, &bw);
	return bw;
}

unsigned int fseek(FILE *fp, unsigned int offset, int origin)
{
	if (fp == stdin || fp == stdout || fp == stderr)
		return 0;
	FIL *ffp = (FIL *)fp;
	FRESULT r;
	if (origin == SEEK_SET)
	{
		r = f_lseek(ffp, offset);
		return offset;
	}
	else if (origin == SEEK_END)
	{
		unsigned int fs = fgetsize(fp);
		r = f_lseek(ffp, fs - offset);
		return fs - offset;
	}
	return 0;
}

int fflush(FILE *fp)
{
	if (fp == stdin || fp == stdout || fp == stderr)
		return 0;
	FIL *ffp = (FIL *)fp;
	FRESULT r = f_sync(ffp);
	return 0;
}

int fputc(int ch, FILE *fp)
{
	if (fp == stdin)
		return 0;
	if (fp == stderr || fp == stdout)
	{
		putchar(ch);
		return 1;
	}
	FIL *ffp = (FIL *)fp;
	int r = f_putc(ch, ffp);
	return r;
}

int fputs(const char *buffer, FILE *fp)
{
	if (fp == stdin)
		return 0;
	if (fp == stderr || fp == stdout)
	{
		puts(buffer);
		return 0;
	}
	FIL *ffp = (FIL *)fp;
	int r = f_puts(buffer, ffp);
	return r;
}

int fgetc(FILE *fp)
{
	char c;
	if (fp == stderr || fp == stdout)
	{
		return 0;
	}
	fread(&c, 1, 1, fp);
	return (int)c;
}

int feof(FILE *fp)
{
	if (fp == stdin || fp == stdout || fp == stderr)
		return 0;
	FIL *ffp = (FIL *)fp;
	int r = f_eof(ffp);
	return r;
}

unsigned int fgetsize(FILE *fp)
{
	if (fp == stdin || fp == stdout || fp == stderr)
		return 0;
	FIL *ffp = (FIL *)fp;
	unsigned int r = f_size(ffp);
	return r;
}
void clearerr(FILE *fp)
{
	FIL *ffp = (FIL *)fp;
	ffp->err = 0;
}

int getline(char **lineptr, size_t *n, FILE *fp)
{
	if ((*lineptr) == NULL)
		(*lineptr) = malloc(1024);
	memset((*lineptr), 0, 1024);
	int count;
	int size = 1024;
	int c = 0;
	for (count = 0; c != '\n' && count < size - 1; count++)
	{
		c = fgetc(fp);
		if (fp == stdin)
		{
			putchar(c);
		}
		int eof = feof(fp);
		if (eof)
		{
			if (count == 0)
				return -1;
			break;
		}

		(*lineptr)[count] = (char)c;
	}

	(*lineptr)[count] = '\0';
	if (n)
	{
		*n = count;
	}
	return count;
}
