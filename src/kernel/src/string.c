#include <system.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>


int memoverlap(void *a, void *b, size_t n)
{
	char *x = a, *y = b;
	int i;
	for (i = 0; i < n; i++)
	{
		if (x + i == y || y + i == x)
			return 1;
	}
	return 0;
}
void *memmove(void *dest, const void *src, size_t nn)
{
	unsigned char *cdest = (unsigned char *)dest;
	unsigned char *csrc = (unsigned char *)src;
	if (memoverlap(cdest, csrc, nn))
	{
		unsigned char *tmp = malloc(nn);
		memcpy(tmp, csrc, nn);
		memcpy(cdest, tmp, nn);
		free(tmp);
	}
	else
	{
		memcpy(cdest, csrc, nn);
	}
	return cdest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	const unsigned char *vptr1 = (const unsigned char *)ptr1;
	const unsigned char *vptr2 = (const unsigned char *)ptr2;
	while (num)
	{
		if (*vptr1 > *vptr2)
			return 1;
		else if (*vptr1 < *vptr2)
			return -1;
		vptr1++;
		vptr2++;
		num--;
	}
	return 0;
}

//
// memcpy
//
void *memcpy(void *dest, const void *src, size_t count)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for (; count != 0; count--)
		*dp++ = *sp++;
	return dest;
}

void *memcpyw(void *dest, const void *src, size_t count)
{
	const unsigned short *sp = (const unsigned short *)src;
	unsigned short *dp = (unsigned short *)dest;
	for (; count != 0; count--)
		*dp++ = *sp++;
	return dest;
}

void *memcpyl(void *dest, const void *src, size_t count)
{
	const unsigned *sp = (const unsigned *)src;
	unsigned *dp = (unsigned *)dest;
	for (; count != 0; count--)
		*dp++ = *sp++;
	return dest;
}

//
// memset
//
void *memset(void *dest, int val, size_t count)
{
	char *temp = (char *)dest;
	for (; count != 0; count--)
		*temp++ = val;
	return dest;
}
unsigned short *memsetw(void *dest, unsigned short val, size_t count)
{
	unsigned short *temp = (unsigned short *)dest;
	for (; count != 0; count--)
		*temp++ = val;
	return dest;
}
unsigned *memsetl(void *dest, unsigned val, size_t count)
{
	unsigned *temp = (unsigned *)dest;
	for (; count != 0; count--)
		*temp++ = val;
	return dest;
}

// Compare two strings. Should return -1 if
// str1 < str2, 0 if they are equal or 1 otherwise.
int strcmp(const char *str1, const char *str2)
{
	int i = 0;
	int failed = 0;
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
		{
			failed = 1;
			break;
		}
		i++;
	}
	// why did the loop exit?
	if ((str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0'))
		failed = 1;

	return failed;
}
int strncmp(const char *str1, const char *str2, size_t n)
{
	int i = 0;
	int failed = 0;
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
		{
			failed = 1;
			break;
		}

		i++;
		if (i >= n)
		{
			failed = 1;
			break;
		}
	}
	// why did the loop exit?
	if ((str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0'))
		failed = 1;

	return failed;
}
// Copy the NULL-terminated string src into dest, and
// return dest.
char *strcpy(char *dest, const char *src)
{
	char *ptr = dest;
	do
	{
		*ptr++ = *src++;
	} while (*src != 0);
	return dest;
}
char *strncpy(char *dest, const char *src, size_t n)
{
	char *ptr = dest;
	size_t count = 0;
	do
	{
		*ptr++ = *src++;
		count++;
	} while (*src != 0 && count < n);
	return dest;
}
char *strchr(const char *s, char c)
{
	while (*s != '\0')
	{
		if (*s == c)
		{
			return (char *)s;
		}
		s++;
	}
	return NULL;
}
char *strrchr(const char *s, int c)
{
	char *rtnval = 0;

	do
	{
		if (*s == c)
			rtnval = (char *)s;
	} while (*s++);
	return (rtnval);
}
char *strcat(char *dest, const char *src)
{
	char *rdest = dest;
	if (strlen(dest) != 0)
	{
		while (*dest)
			dest++;
	}
	while (*dest++ = *src++)
		;
	return rdest;
}
size_t strlen(const char *str)
{
	size_t retval;
	for (retval = 0; *str != '\0'; str++)
		retval++;
	return retval;
}
char *strtok(char *s, const char *delm)
{
	static int currIndex = 0;
	if (!s || !delm || s[currIndex] == '\0')
		return NULL;

	char *W = (char *)malloc(sizeof(char) * 100);
	int i = currIndex, k = 0, j = 0;

	while (s[i] != '\0')
	{
		j = 0;
		while (delm[j] != '\0')
		{
			if (s[i] != delm[j])
				W[k] = s[i];
			else
				goto It;
			j++;
		}
		i++;
		k++;
	}
It:
	W[i] = 0;
	currIndex = i + 1;
	return W;
}
char *strdup(char *src)
{
	char *str;
	char *p;
	int len = 0;

	while (src[len])
		len++;
	str = malloc(len + 1);
	p = str;
	while (*src)
		*p++ = *src++;
	*p = '\0';
	return str;
}
char *strstr(const char *s1, const char *s2)
{
	const char *a = s1, *b = s2;
	for (;;)
	{
		if (!*b)
			return (char *)a;
		else if (!*a)
			return NULL;
		else if (*a++ != *b++)
		{
			a = ++s1;
			b = s2;
		}
	}
}
size_t strcspn(const char *s1, const char *s2)
{
	const char *sc1, *sc2;
	for (sc1 = s1; *sc1 != '\0'; ++sc1)
	{
		for (sc2 = s2; *sc2 != '\0'; ++sc2)
		{
			if (*sc1 == *sc2)
				return (sc1 - s1);
		}
	}
	return (sc1 - s1); // terminating nulls match.
}

int32_t string_split(const char *txt, char delim, char ***tokens)
{
	int32_t *tklen, *t, count = 1;
	char **arr, *p = (char *)txt;

	while (*p != '\0')
	{
		if (*p++ == delim)
		{
			count += 1;
		}
	}
	t = tklen = (int32_t *)calloc(count, sizeof(int32_t));
	//	printf("ss alloc1 0x%08X\n",t);
	for (p = (char *)txt; *p != '\0'; p++)
	{
		*p == delim ? *t++ : (*t)++;
	}
	*tokens = arr = (char **)malloc(count * sizeof(char *));
	//	printf("ss alloc2 0x%08X\n",arr);
	t = tklen;
	p = *arr++ = (char *)calloc(*(t++) + 1, sizeof(char *));
	//	printf("ss alloc3 0x%08X\n",p);
	int icount = 0;
	while (*txt != '\0')
	{
		if (*txt == delim)
		{
			//*p++ = delim;
			p = *arr++ = (char *)calloc(*(t++) + 1, sizeof(char *));
			//	printf("ss alloc%d 0x%08X\n",icount,p);
			txt++;
		}
		else
		{
			*p++ = *txt++;
		}
		icount++;
	}
	// printf("ss free 0x%08X\n",tklen);
	free(tklen);
	// printf("ss free 0x%08X\n",*tokens);
	return count;
}
