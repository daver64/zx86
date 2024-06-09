/*

   Copyright 2009 Pierre KRIEGER

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
*/

#ifndef _STDC_STDIO_H_
#define _STDC_STDIO_H_

#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int	snprintf(char* s, size_t n, const char* format, ...);
int	sprintf(char* s, const char* format, ...);
int	sscanf(char* s, char* format, ...);
int scanf(char *fmt, ...);
int	vsnprintf(char* s, size_t n, const char* format, ...);
int	vsprintf(char* s, const char* format, ...);
int	vsscanf(char* s, char* format, ...);

typedef void FILE;

int getline(char** lineptr,size_t* n,FILE* fp);
FILE* fopen(const char* filename, const char* mode);
FILE *path_fopen(const char *filename, const char *mode);
int fclose(FILE* fp);
unsigned int ftell(FILE* fp);
unsigned int fread(void* buffer,unsigned int size,unsigned int count, FILE* fp);
unsigned int fwrite(void* buffer,unsigned int size,unsigned int count, FILE* fp);
unsigned int fseek(FILE* fp,unsigned int offset, int origin);
int fflush(FILE* fp);
int fputc(int ch, FILE* fp);
int fputs(const char* buffer, FILE* fp);
int fgetc(FILE* fp);
int fprintf(FILE* fp, const char* format, ...);
int feof(FILE* fp);
void clearerr(FILE *fp);
int printf(const char *format, ...);
int sprintf(char *out, const char *format, ...);
int puts(const char *text);
void putch(unsigned char c);
int putchar (int c);
int getchar();
int unlink(const char *filename);
void ungetc(int k,FILE* fp);
unsigned int fgetsize(FILE* fp);
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;


#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

#define FILENAME_MAX 260
#define FOPEN_MAX 20

#define EOF (-1)

#ifdef __cplusplus
}
#endif


#endif
