#ifndef _SYSTEM_SHELL_H_
#define _SYSTEM_SHELL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
/* SHELL.C */
void get_next_character();
void skip_ws();
char *read_label();
char *read_digits();
char *read_hexdigits();
int32_t read_number();
int32_t read_hexnumber();
int32_t sh_getline(char *buffer, int32_t buflen);
int isaddop(char c);
int ismulop(char c);
int getline(char **lineptr, size_t *n, FILE *fp);
void disasm(unsigned char *code, size_t codelength);
#ifdef __cplusplus
}
#endif
#endif
