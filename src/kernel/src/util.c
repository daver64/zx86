#include <stddef.h>
#include <system.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>

#include <bget.h>

uint32_t next_pow2(uint32_t x)
{
	return x == 1 ? 1 : 1 << (32 - __builtin_clz(x - 1));
}

size_t write(int fd, const void *buffer, size_t count)
{
	char *buf = (char *)buffer;
	for (int i = 0; i < count; i++)
		putchar(buf[i]);
	return count;
}

static int exitcalled = 0;
void exit(int r)
{
	exitcalled = 1 + r;
}

unsigned char inportb(unsigned short _port)
{
	unsigned char rv;
	__asm__ __volatile__("inb %1, %0" : "=a"(rv) : "dN"(_port));
	return rv;
}
unsigned short inportw(unsigned short _port)
{
	unsigned short rv;
	asm volatile("inw %1, %0" : "=a"(rv) : "dN"(_port));
	return rv;
}
void outportb(unsigned short _port, unsigned char _data)
{
	asm volatile("outb %1, %0" : : "dN"(_port), "a"(_data));
}
void outportw(unsigned short _port, unsigned short _data)
{
	asm volatile("outw %1, %0" : : "dN"(_port), "a"(_data));
}
uint32_t inportl(uint16_t portid)
{
	uint32_t ret;
	asm volatile("inl %%dx, %%eax" : "=a"(ret) : "d"(portid));
	return ret;
}
void outportl(uint16_t portid, uint32_t value)
{
	asm volatile("outl %%eax, %%dx" : : "d"(portid), "a"(value));
}
void enable_interrupts()
{
	__asm__ __volatile__("sti");
}
void disable_interrupts()
{
	__asm__ __volatile__("cli");
}
void halt_cpu()
{
	__asm__ __volatile__("hlt");
}
void pause_cpu()
{
	__asm__ __volatile__("pause");
}

// optimize("no-omit-frame-pointer") doesn't seem to work
// we still don't get a frame-point unless we force -O0 for the function with optimize(0)
__attribute__((noinline, noclone, returns_twice, optimize(0)))
int setjmp(jmp_buf var){
    // relies on the compiler to make a stack-frame
    // because we're using inline asm inside a function instead of at global scope
     __asm__(
             "    mov 8(%ebp), %eax     # get pointer to jmp_buf, passed as argument on stack\n"
             "    mov    %ebx, (%eax)   # jmp_buf[0] = ebx\n"
             "    mov    %esi, 4(%eax)  # jmp_buf[1] = esi\n"
             "    mov    %edi, 8(%eax)  # jmp_buf[2] = edi\n"
             "    mov    (%ebp), %ecx\n"
             "    mov    %ecx, 12(%eax) # jmp_buf[3] = ebp\n"
             "    lea    8(%ebp), %ecx  # get previous value of esp, before call\n"
             "    mov    %ecx, 16(%eax) # jmp_buf[4] = esp before call\n"
             "    mov    4(%ebp), %ecx  # get saved caller eip from top of stack\n"
             "    mov    %ecx, 20(%eax) #jmp_buf[5] = saved eip\n"
             "    xor    %eax, %eax     #eax = 0\n"
     );

    return 0;
}

__attribute__((noinline, noclone, optimize(0)))
void longjmp(jmp_buf var,int m){
    __asm__("    mov  8(%ebp),%edx # get pointer to jmp_buf, passed as argument 1 on stack\n"
            "    mov  12(%ebp),%eax #get int val in eax, passed as argument 2 on stack\n"
            "    test    %eax,%eax # is int val == 0?\n"
            "    jnz 1f\n"
            "    inc     %eax      # if so, eax++\n"
            "1:\n"
            "    mov   (%edx),%ebx # ebx = jmp_buf[0]\n"
            "    mov  4(%edx),%esi # esi = jmp_buf[1]\n"
            "    mov  8(%edx),%edi #edi = jmp_buf[2]\n"
            "    mov 12(%edx),%ebp # ebp = jmp_buf[3]\n"
            "    mov 16(%edx),%ecx # ecx = jmp_buf[4]\n"
            "    mov     %ecx,%esp # esp = ecx\n"
            "    mov 20(%edx),%ecx # ecx = jmp_buf[5]\n"
            "    jmp *%ecx         # eip = ecx");
}

void disasm(unsigned char *code, size_t codelength)
{
	unsigned char *code_ptr = code;
	uint32_t address = (uint32_t)code_ptr;
	printf("Dissassembly 0x%08X:\n", address);
	for (size_t i = 0; i < codelength; i++)
	{

		// printf("%08X ",address);
		switch (*code_ptr)
		{
		case 0x50:
		{
			printf("push eax");
		}
		break;
		case 0x53:
		{
			printf("push ebx");
		}
		break;
		case 0x5b:
		{
			printf("pop ebx");
		}
		break;
		case 0x5d:
		{
			printf("pop ebp");
		}
		break;
		case 0x55:
		{
			printf("push ebp");
		}
		break;
		case 0x58:
		{
			printf("pop eax");
		}
		break;
		case 0xff:
		{
			printf("call ");
			code_ptr++;
			i++;
			switch (*code_ptr)
			{
			case 0xD0:
			{
				printf("eax");
			}
			break;
			case 0x10:
			{
				printf("[eax]");
			}
			break;
			default:
			{
				printf("db %02X ", *code_ptr);
			}
			break;
			}
		}
		break;
		case 0x89:
		{
			unsigned char source = *code_ptr;
			code_ptr++;
			i++;
			unsigned char destination = *code_ptr;
			printf("mov ");
			switch (destination)
			{
			case 0x06:
			{
				printf("[esi],eax");
			}
			break;
			case 0xe5:
			{
				printf("ebp,esp");
			}
			break;
			case 0x07:
			{
				printf("[edi],eax");
			}
			break;
			default:
			{
				printf("db %02X ", destination);
			}
			break;
			}
		}
		break;
		case 0x8B:
		{
			printf("mov eax,[");
			code_ptr++;
			i++;
			switch (*code_ptr)
			{
			case 0x06:
			{
				printf("esi]");
			}
			break;
			case 0x07:
			{
				printf("edi]");
			}
			break;
			default:
			{
				printf("db %02X ",*code_ptr);
			}
			break;
			}
		}
		break;
		case 0x01:
		{
			printf("add eax,");
			code_ptr++;
			i++;
			switch (*code_ptr)
			{
			case 0xD8:
			{
				printf("ebx");
			}
			break;

			default:
			{
				printf("db %02X ", *code_ptr);
			}
			break;
			}
		}
		break;
		case 0x81:
		{
			printf("add ");
			code_ptr++;
			i++;
			switch (*code_ptr)
			{
			case 0xc7:
			{
				printf(" edi,");
			}
			break;

			default:
			{
				printf("db %02X ", *code_ptr);
			}
			break;
			}
			union
			{
				unsigned char uc[4];
				uint32_t v[1];
			} u;
			code_ptr++;
			i++;
			u.uc[0] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[1] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[2] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[3] = *code_ptr;
			printf("0x%08X", u.v[0]);
		}
		break;
		case 0xB8:
		{
			printf("mov eax,");
			union
			{
				unsigned char uc[4];
				uint32_t v[1];
			} u;
			code_ptr++;
			i++;
			u.uc[0] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[1] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[2] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[3] = *code_ptr;
			printf("0x%08X", u.v[0]);
		}
		break;
		case 0xBB:
		{
			printf("mov eax,");
			union
			{
				unsigned char uc[4];
				uint32_t v[1];
			} u;
			code_ptr++;
			i++;
			u.uc[0] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[1] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[2] = *code_ptr;
			code_ptr++;
			i++;
			u.uc[3] = *code_ptr;
			printf("0x%08X", u.v[0]);
		}
		break;
		case 0xC3:
		{
			printf("ret");
		}
		break;
		case 0xF7:
		{
			printf("mul ");
			code_ptr++;
			i++;
			switch (*code_ptr)
			{
			case 0xE3:
			{
				printf("ebx");
			}
			break;

			default:
			{
				printf("db %02X", *code_ptr);
			}
			break;
			}
		}
		break;
		default:
		{
			printf("db %02X ", *code_ptr);
		}
		break;
		}
		code_ptr++;
		printf("\n");
	}
}
