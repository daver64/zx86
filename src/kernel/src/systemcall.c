#include<system.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#define OS_GETCH 0x01
#define OS_PUTCH 0x02
#define OS_PUTS 0x03
#define OS_GETS 0x04
#define OS_FOPEN 0x05
#define OS_FREAD 0x06
#define OS_FWRITE 0x07
#define OS_FCLOSE 0x08
#define OS_FTELL 0x09
#define OS_FSEEK 0x0A
#define OS_MALLOC 0x0B
#define OS_FREE 0x0C
#define OS_CLS 0x0D
#define OS_GOTOXY 0x0E
#define OS_GETXY 0x0F
#define OS_ATTRIB 0x10
#define OS_HDC 0x11
#define OS_MOUNT 0x12
#define OS_UNMOUNT 0x13
#define OS_THREAD_TERMINATE 0x14


int pop_keypress();
void os_syscall(struct regs* r)
{
	switch(r->eax)
	{
		case OS_GETCH:
			r->eax=kbhit();
			if(r->eax)
				pop_keypress();
			break;

		case OS_PUTCH:
			putchar(r->ebx);
			r->eax=0;
			break;

		case OS_PUTS:
			puts((const char*)r->ebx);
			r->eax=0;
			break;

		case OS_FOPEN:
			r->eax=(unsigned int)fopen((const char*)r->ebx,(const char*)r->ecx);
			break;

		case OS_FCLOSE:
			fclose((FILE*)r->ebx);
			r->eax=0;
			break;

		case OS_FTELL:
			r->eax=ftell((FILE*)r->ebx);
			break;

		case OS_FSEEK:
			r->eax=fseek((FILE*)r->ebx,r->ecx,r->edx);
			break;

		case OS_FREAD:
			r->eax=fread((char*)r->ebx,r->ecx,r->edx,(FILE*)r->edi);
			break;

		case OS_FWRITE:
			r->eax=fwrite((char*)r->ebx,r->ecx,r->edx,(FILE*)r->edi);
			break;

		case OS_MALLOC:
			r->eax=(unsigned int)malloc(r->ebx);
			break;

		case OS_FREE:
			free((void*)r->ebx);
			r->eax=0;
			break;
			
		default:
			printf("unknown syscall:\neax=%08X\n",r->eax);
			printf("ebx=%08X\n",r->ebx);
			printf("ecx=%08X\n",r->ecx);
			printf("edx=%08X\n",r->edx);
			printf("edi=%08X\n",r->edi);
			printf("esi=%08X\n",r->esi);
			printf("ebp=%08X\n",r->ebp);
			printf("esp=%08X\n",r->esp);
			printf("eip=%08X\n",r->eip);			
			break;
	}
} 
