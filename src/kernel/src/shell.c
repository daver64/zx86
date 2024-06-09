#include <system.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <bget.h>
#include <ff.h>
#include <scheme-private.h>

typedef int32_t (*user_f_t)();

int64_t get_tick_count();
extern void beep(uint32_t frequency, int32_t duration_ticks);
void set_text_attrib(unsigned char attr);

unsigned short inportw(unsigned short _port);
const char *asciitime();
int exec_at_buffer(unsigned char *buffer);
void dfs_report_space();
int dfs_cd(int argc, char **argv);
int dfs_mkdir(int argc, char **argv);
int dfs_rmdir(int argc, char **argv);
int dfs_fdisc(int argc, char **argv);
int dfs_ls(int argc, char **argv);
int dfs_mkfs(int argc, char **argv);
int dfs_cp(int argc, char **argv);
int dfs_rm(int argc, char **argv);
int dfs_mv(int argc, char **argv);

int sh_test(int argc, char **args);
int sh_dump(int argc, char **args);
int sh_cls(int argc, char **args);
int sh_kmesg(int argc, char **args);
int sh_shutdown(int argc, char **argv);
int sh_bgc(int argc, char **argv);
int sh_fgc(int argc, char **argv);
int sh_mount(int argc, char **argv);
int sh_cat(int argc, char **argv);
int sh_bcat(int argc, char **argv);
int sh_df(int argc, char **argv);
int sh_pause(int argc, char **argv);
int sh_ticks(int argc, char **argv);
int sh_attrib(int argc, char **argv);
int sh_datetime(int argc, char **argv);
int sh_numbase(int argc, char **argv);
int sh_help(int argc, char **argv);
int sh_kb(int argc, char **argv);
int sh_printexitcode(int argc, char **argv);
int sh_mem(int argc, char **argv);
int sh_smp(int argc, char **argv);
int sc_main(int argc, char **argv);
int fasm_main(int argc, char **argv);
int kilo_main(int argc, char **argv);

int sh_binload(int argc, char **argv);
int sh_binsave(int argc, char **argv);
int sh_dumpmem(int argc, char **argv);
int sh_pokemem(int argc, char **argv);
int sh_peekmem(int argc, char **argv);
int sh_disasm(int argc, char **argv);
int sh_call(int argc, char **argv);
extern void kmesg();

char prompt[32];
uint32_t wantquit = false;
int ctok = 0;
int shell_num_base = 10;
int print_exit_code = true;
int32_t allowpromptoverwrite = false;
extern uint32_t scratchbuffer_start;

typedef struct shellcommand
{
	const char *cmd;
	const char *description;
	int (*function_t)(int, char **);
} shellcommand_t;

shellcommand_t cmds[] =
	{
		{"test", "current WIP testing. perilous (:", &sh_test},
		{"cls", "clears the screen", &sh_cls},
		{"ls", "lists files", &dfs_ls},
		{"kmesg", "print kernel boot message", &sh_kmesg},
		{"shutdown", "shut down the computer - qemu only currently", &sh_shutdown},
		{"bgcolour", "set background colour", &sh_bgc},
		{"fgcolour", "set foreground colour", &sh_fgc},
		{"mount", "mount disc drive as root fs", &sh_mount},
		{"fdisc", "create and write primary partition to primary disc", &dfs_fdisc},
		{"fasm", "run assembler on file", &fasm_main},
		{"ed", "text editor", &kilo_main},
		{"mkfs", "formats disc partition", &dfs_mkfs},
		{"cp", "copies files", &dfs_cp},
		{"mv", "moves files", &dfs_mv},
		{"df", "display filesystem usage", &sh_df},
		{"cat", "print contents of file to screen", &sh_cat},
		{"bcat", "print binary contents of file to screen", &sh_bcat},
		{"rm", "remove files", &dfs_rm},
		{"mkdir", "make directory", &dfs_mkdir},
		{"rmdir", "remove empty directory", &dfs_rmdir},
		{"cd", "change current directory", &dfs_cd},
		{"pause", "pause for n milliseconds", &sh_pause},
		{"ticks", "print number of ticks since kernel start", &sh_ticks},
		{"attrib", "set display attribs", &sh_attrib},
		{"datetime", "print the date and time", &sh_datetime},
		{"numbase", "set the number base used by the shell", &sh_numbase},
		{"kb", "read and display keyboard data", &sh_kb},
		{"printexit", "print exit code", &sh_printexitcode},
		{"mem", "print heap stats", &sh_mem},
		{"smp", "boot up additional cores", &sh_smp},
		{"scheme", "tinyscheme interpreter", &sc_main},
		{"load", "load file into memory", &sh_binload},
		{"save", "save memory into file", &sh_binsave},
		{"peek", "peek at memory values", &sh_peekmem},
		{"poke", "poke values into memory", &sh_pokemem},
		{"dump", "dump memory values to console", &sh_dumpmem},
		{"disasm", "disassemble", &sh_disasm},
		{"call", "call function at address", &sh_call},
		{"help", "get some help", &sh_help}};

size_t sh_num_cmds()
{
	return sizeof(cmds) / sizeof(shellcommand_t);
}

/*
 *

 ____  _          _ _
 / ___|| |__   ___| | |
 \___ \| '_ \ / _ \ | |
 ___) | | | |  __/ | |
 |____/|_| |_|\___|_|_|


 *
 */
extern int cpp_main(int argc, char *args[]);
void smp_init();

int sh_test(int argc, char **args)
{
	int result =cpp_main(argc, args);




	return result;
}
int sh_binload(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("usage:\nbinload filename address\n");
		return 1;
	}

	char *fbuffer = NULL;
	size_t fbuffer_len = 0;
	uint32_t load_address = 0;
	for (int arg = 0; arg < argc; arg++)
	{
		if (arg == 1)
		{
			FILE *fp = fopen(argv[arg], "r");
			if (fp)
			{
				fbuffer_len = fgetsize(fp);
				fbuffer = malloc(fbuffer_len);
				fread(fbuffer, 1, fbuffer_len, fp);
				fclose(fp);
			}
			else
			{
				printf("unable to open file %s\n", argv[arg]);
				return 1;
			}
		}
		else if (arg == 2)
		{
			load_address = strtol(argv[arg], 0, 16);
			// load_address=atoi(argv[arg]);
		}
	}

	if (load_address > 0 && fbuffer_len > 0)
	{
		printf("load address 0x%08x , buffer length %u\n", load_address, fbuffer_len);
		memcpy((void *)load_address, (void *)fbuffer, fbuffer_len);
		free(fbuffer);
	}
	else
	{
		printf("incorrect load address 0x%08x or buffer length %u\n", load_address, fbuffer_len);
		return 1;
	}
	return 0;
}
int sh_binsave(int argc, char **argv)
{
	if (argc < 4)
	{
		printf("usage:\nbinsave filename address length\n");
		return 1;
	}
	FILE *fp = NULL;
	void *address = NULL;
	size_t length = 0;
	for (int arg = 0; arg < argc; arg++)
	{
		if (arg == 1)
		{
			fp = fopen(argv[arg], "w");
			if (!fp)
			{
				printf("unable to open file %s\n", argv[arg]);
				return 1;
			}
		}
		if (arg == 2)
		{
			address = (void *)(strtol(argv[arg], 0, 16));
		}
		if (arg == 3)
		{
			length = strtol(argv[arg], 0, 10);
		}
	}
	if (address != NULL && fp != NULL && length > 0)
	{
		fwrite((void *)address, 1, length, fp);
		fclose(fp);
	}
	else
	{
		printf("parameter error\n");
		return 1;
	}
	return 0;
}
int sh_dumpmem(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("usage:\ndumpmem address num-16-byte-paragraphs\n");
		return 1;
	}
	uint8_t *address = NULL;
	int numparas = 0;
	for (int arg = 0; arg < argc; arg++)
	{
		if (arg == 1)
		{
			address = (uint8_t *)(strtol(argv[arg], 0, 16));
		}
		if (arg == 2)
		{
			numparas = strtol(argv[arg], 0, 10);
		}
	}
	if (address != NULL && numparas > 0)
	{
		int numbytes = numparas * 16;
		char *asciibuffer = malloc(17);
		memset(asciibuffer, 0, 17);
		for (int para = 0; para < numparas; para++)
		{
			char buf[2] = {0, 0};

			for (int i = 0; i < 16; i++)
			{
				uint8_t c = *(uint8_t *)(address + ((para * 16) + i));
				printf("%02X ", c);
			}
			printf("\t|");
			for (int i = 0; i < 16; i++)
			{
				uint8_t c = *(uint8_t *)(address + ((para * 16) + i));
				if (isprint(c) && c != '\n' && c != '\r' && c != '\t')
				{
					printf("%c", c);
				}
				else
				{
					printf(".");
				}
			}
			printf("|\n");
		}
		free(asciibuffer);
		printf("\n");
	}
	else
	{
		printf("parameter error\n");
		return 1;
	}
	return 0;
}
int sh_pokemem(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("usage:\npokemem address byte\n");
		return 1;
	}
	uint8_t *address = NULL;
	uint8_t byte = 0;
	for (int arg = 0; arg < argc; arg++)
	{
		if (arg == 1)
		{
			address = (uint8_t *)(strtol(argv[arg], 0, 16));
		}
		if (arg == 2)
		{
			byte = strtol(argv[arg], 0, 16);
		}
	}
	if (address != NULL)
	{
		*address = byte;
	}
	else
	{
		printf("parameter error\n");
	}
	return 0;
}
int sh_peekmem(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage:\npeekmem address\n");
		return 1;
	}
	uint8_t *address = NULL;
	for (int arg = 0; arg < argc; arg++)
	{
		if (arg == 1)
		{
			address = (uint8_t *)(strtol(argv[arg], 0, 16));
		}
	}
	if (address != NULL)
	{
		printf("%08X\n", *address);
	}
	else
	{
		printf("parameter error\n");
	}
	return 0;
}
int sh_disasm(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("usage:\ndisasm <address> <length>\n");
		return 1;
	}
	unsigned char *code = 0;
	size_t codelength = 0;

	for (int arg = 0; arg < argc; arg++)
	{
		if (arg == 1)
		{
			code = (unsigned char *)(strtol(argv[arg], 0, 16));
		}
		if (arg == 2)
		{
			codelength = strtol(argv[arg], 0, 10);
		}
	}
	if (code != NULL && codelength > 0)
	{
		disasm(code, codelength);
	}
	else
	{
		printf("parameter error\n");
		return 1;
	}
}
int sh_call(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage:\ncall <address>\n");
		return 1;
	}
	unsigned char *buffer = (unsigned char *)(strtol(argv[1], 0, 16));
	if (buffer)
	{
		exec_at_buffer(buffer);
	}
}
int sh_smp(int argc, char **args)
{
	int result = cpp_main(argc, args);
	smp_init();
	return result;
}
int exec_at_buffer(unsigned char *buffer)
{
	user_f_t f = (user_f_t)buffer;
	uint32_t result = f();
	if (print_exit_code)
	{
		printf("%d\n", result);
	}
}
int sh_mem(int argc, char **args)
{
	print_mem_stats();
	print_heap_stats();
	return 0;
}
int sh_kb(int argc, char **args)
{
	printf("keyboard probe, press ESC to quit\n");
	int k = 0;
	while (k != 27)
	{
		k = getchar();
		printf("%03d 0x%02x '%c'\n", k, k, k);
	}
	return 0;
}
int sh_printexitcode(int argc, char **args)
{
	if (print_exit_code == false)
		print_exit_code = true;
	else
		print_exit_code = false;
	return 0;
}
int sh_ticks(int argc, char **args)
{
	uint32_t div = 1;
	uint32_t num_ticks = (uint32_t)get_tick_count();
	if (argc == 2)
	{
		div = strtoul(args[1], NULL, 10);
	}
	printf("%u\n", num_ticks / div);
	return 0;
}
int sh_datetime(int argc, char **args)
{
	printf("[%s]\n", asciitime());
	int u = time(0);
	printf("[%d]\n", u);
	return 0;
}
void screen_pause(const char *msg)
{
	uint8_t ofgc = get_foreground_colour();
	set_foreground_colour(VGA_YELLOW);
	printf("%s\n", msg);
	set_foreground_colour(ofgc);
	while (!kbhit())
	{
	}
	int k = getchar();
}
int sh_help(int argc, char **args)
{
	cls();
	int i;
	int count = 0;
	for (i = 0; i < sh_num_cmds(); i++)
	{
		printf("%-20s:%s\n", cmds[i].cmd, cmds[i].description);
		if (count == get_num_rows() - 3)
		{
			count = 0;
			screen_pause("press any key to continue");
		}
		count++;
	}
	return 0;
}
int sh_pause(int argc, char **args)
{
	if (argc != 2)
	{
		printf("usage:\npause <millisec>\n");
		return 1;
	}
	int period = atoi(args[1]);
	if (period > 0)
		wait(period);
}
int sh_numbase(int argc, char **args)
{
	if (argc == 1)
	{
		printf("%d\n", shell_num_base);
		return 0;
	}
	if (argc != 2)
	{
		printf("usage:\nnumbase <num>\n");
		return 2;
	}
	int nb = atoi(args[1]);
	if (nb == 2 || nb == 10 || nb == 16)
		shell_num_base = nb;
	else
	{
		printf("base must be 2,10 or 16\n");
		return 2;
	}
	return 0;
}
int sh_kmesg(int argc, char **args)
{
	kmesg();
	return 0;
}
int sh_cls(int argc, char **args)
{
	cls();
	return 0;
}
int sh_shutdown(int argc, char **args)
{
	// beep(440,250);
	outportw(0x604, 0x2000);
	//	acpi_shutdown();
}
int sh_bgc(int argc, char **args)
{
	if (argc != 2)
	{
		printf("usage:\nbgc <colour>\n");
		return 1;
	}
	uint32_t colour = strtoul(args[1], NULL, 16); // atoi(args[1]);
	if (colour <= 0x0F)
	{
		set_background_colour(colour);
		cls();
	}
	return 0;
}
int sh_attrib(int argc, char **args)
{
	if (argc != 2)
	{
		printf("usage:\nattrib <val>\n");
		return 1;
	}
	unsigned char attrib = (unsigned char)strtoul(args[1], NULL, 16);
	set_text_attrib(attrib);
	return 0;
}
int sh_fgc(int argc, char **args)
{
	if (argc != 2)
	{
		printf("usage:\nbgc <colour>\n");
		return 1;
	}
	uint32_t colour = strtoul(args[1], NULL, 16);
	if (colour <= 0x0F)
	{
		set_foreground_colour(colour);
		cls();
	}
}
int sh_mount(int argc, char **argv)
{
	dfs_mount();
	return 0;
}
int sh_df(int argc, char **argv)
{
	dfs_report_space();
	return 0;
}
int sh_cat(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage:\ncat <filename>\n");
		return 1;
	}
	FILE *fp = fopen(argv[1], "r");
	if (fp)
	{
		unsigned int len = fgetsize(fp);
		char *buffer = malloc(len + 1);
		memset(buffer, 0, len + 1);
		fread(buffer, 1, len, fp);
		printf("%s\n", buffer);
		fclose(fp);
		free(buffer);
	}
	int result = 0;
	return result;
}
int sh_bcat(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage:\nbcat <filename>\n");
		return 1;
	}
	FILE *fp = fopen(argv[1], "r");
	if (fp)
	{
		unsigned int len = fgetsize(fp);
		char *buffer = malloc(len + 1);
		memset(buffer, 0, len + 1);
		fread(buffer, 1, len, fp);
		fclose(fp);
		unsigned char *code_ptr = (unsigned char *)buffer;
		int i;
		uint8_t ofgc = get_foreground_colour();
		set_foreground_colour(VGA_YELLOW);
		for (i = 0; i < len; i++)
		{
			if (i > 0 && i % 16 == 0)
				putchar('\n');
			if (i > 0 && i % 256 == 0)
				putchar('\n');
			printf("%02X ", *code_ptr);
			code_ptr++;
		}
		putchar('\n');
		set_foreground_colour(ofgc);
		free(buffer);
	}
	int result = 0;
	return result;
}

#define LINE_BUFFER_LEN (4096)
int plen = 0;

int shell_exec(const char *filename)
{
	int recognised_command = false;
	char *exe_filename = malloc(128);
	char *com_filename = malloc(128);
	snprintf(exe_filename, 128, "%s.elf", filename);
	snprintf(com_filename, 128, "%s.com", filename);
	FILE *fp = path_fopen(exe_filename, "r");
	if (fp)
	{
		fclose(fp);
		int result = elf_quickload(exe_filename);
		if (result == 0)
			recognised_command = true;
	}
	else
	{
		fp = path_fopen(com_filename, "r");
		if (fp)
		{
			unsigned int len = fgetsize(fp);
			unsigned char *buffer = malloc(len + 1);
			memset(buffer, 0, len + 1);
			fread(buffer, 1, len, fp);
			fclose(fp);
			exec_at_buffer(buffer);
			recognised_command = true;
			free(buffer);
		}
	}
	free(exe_filename);
	free(com_filename);
	return recognised_command;
}
void shell_main()
{
	f_chdir("\\user\\dave");
	char *linebuffer = malloc(LINE_BUFFER_LEN);
	while (!wantquit)
	{
		char cdbuf[256];
		memset(cdbuf, 0, 256);
		f_getcwd(&cdbuf[0], 128);
		for (int i = 0; i < 256; i++)
		{
			cdbuf[i] = tolower(cdbuf[i]);
		}
		plen = strlen(cdbuf);
		int ofgc = get_foreground_colour();
		set_foreground_colour(VGA_LIGHTGREY);
		printf("%s", cdbuf);
		set_foreground_colour(ofgc);
		sprintf(prompt, "%s", ">");
		printf(prompt);

		memset(linebuffer, 0, LINE_BUFFER_LEN);
		int len = sh_getline(linebuffer, LINE_BUFFER_LEN);

		char *cmdstart_ptr = linebuffer;
		while (*cmdstart_ptr != 0)
		{
			if (*cmdstart_ptr == '\t')
				*cmdstart_ptr = ' ';
			cmdstart_ptr++;
		}
		cmdstart_ptr = linebuffer;
		while (isspace(*cmdstart_ptr))
		{
			cmdstart_ptr++;
		}

		char **args;
		int argc;
		argc = string_split(cmdstart_ptr, ' ', &args);
		unsigned int i = 0;
		int recognised_command = false;
		for (i = 0; i < sh_num_cmds(); i++)
		{
			if (!strcmp(args[0], cmds[i].cmd))
			{
				recognised_command = true;
				int result = (*cmds[i].function_t)(argc, args);
			}
		}

		if (!recognised_command && strlen(args[0]) > 0)
		{
			recognised_command = shell_exec(args[0]);
		}
		if (!recognised_command && strlen(args[0]) > 0)
		{
			printf("unrecognised command [");
			char *cptr = args[0];
			while (*cptr != '\0')
			{
				if (*cptr == 27)
					printf("ESC");
				else
					putchar(*cptr);
				cptr++;
			}
			printf("]\n");
		}

		for (i = 0; i < argc; i++)
		{
			free(*(args + i));
		}
		free(args);
	}
}

/*
 *
 ____                       _____           _
 |  _ \ __ _ _ __ ___  ___  |_   _|__   ___ | |___
 | |_) / _` | '__/ __|/ _ \   | |/ _ \ / _ \| / __|
 |  __/ (_| | |  \__ \  __/   | | (_) | (_) | \__ \
 |_|   \__,_|_|  |___/\___|   |_|\___/ \___/|_|___/

 *
 */

int plen_active = true;
void cputchar(int c)
{
	if (c < 128)
		putchar(c);
}
void get_next_character()
{
	ctok = getchar();
	if (!allowpromptoverwrite)
	{
		int len = strlen(prompt); //+plen;
		if (plen_active)
			len += plen;
		int cpos = get_cursor_x();
		if (ctok == 0x08 && len == cpos)
			return;
		else
			cputchar(ctok);
	}
	else
		cputchar(ctok);
}

void skip_ws()
{
	while (isspace(ctok))
		get_next_character();
}

char *read_label()
{
#define MAX_LABEL (256)
	char *labelbuffer = (char *)malloc(MAX_LABEL);
	memset(labelbuffer, 0, MAX_LABEL);
	size_t count = 0;
	while (ctok >= 'a' && ctok <= 'z' && count < MAX_LABEL)
	{
		labelbuffer[count] = ctok;
		count++;
		get_next_character();
	}
	return labelbuffer;
}

#define MAX_DIGITS (256)
char *read_digits()
{
	char *digitsbuffer = (char *)malloc(MAX_DIGITS);
	memset(digitsbuffer, 0, MAX_DIGITS);
	size_t count = 0;
	while (ctok >= '0' && ctok <= '9' && count < MAX_DIGITS)
	{
		digitsbuffer[count] = ctok;
		count++;
		get_next_character();
	}
	return digitsbuffer;
}

char *read_hexdigits()
{
	char *digitsbuffer = (char *)malloc(MAX_DIGITS);
	memset(digitsbuffer, 0, MAX_DIGITS);
	size_t count = 0;
	while (((ctok >= '0' && ctok <= '9') || (toupper(ctok) >= 'A' && toupper(ctok) <= 'F')) && count < MAX_DIGITS)
	{
		digitsbuffer[count] = ctok;
		count++;
		get_next_character();
	}
	return digitsbuffer;
}

int32_t read_number()
{
	char *digits = read_digits();
	int32_t number = strtol(digits, NULL, 10);
	free(digits);
	return number;
}

int32_t read_hexnumber()
{
	char *hexdigits = read_hexdigits();
	int32_t number = strtol(hexdigits, NULL, 16);
	free(hexdigits);
	return number;
}

char **command_history = NULL;
#define COMMAND_HISTORY_SIZE (128)
int32_t h_index = -1;
int32_t sh_getline(char *buffer, int32_t buflen)
{
	int32_t count = 0;
	ctok = 0;
	while (ctok != '\n' && count < buflen)
	{
		get_next_character();
		if (ctok == VK_UP)
		{
			char *b = *(command_history + h_index);
			if(!b)
			{
				h_index=0;
			}
			b=*(command_history + h_index);
			if (b)
			{
				int aplen = strlen(prompt) + plen;
				int px = get_cursor_x();
				while (px > aplen)
				{
					putchar('\b');
					px--;
				}
				snprintf(buffer, buflen, "%s", b);
				printf(buffer);
				count = strlen(buffer);
				h_index++;
			}
		}
		if (ctok != '\n' && ctok != '\b' && ctok != 27 && ctok < 128)
		{
			buffer[count] = ctok;
			count++;
		}
		else if (ctok == '\b' && count > 0)
		{
			count--;
			buffer[count] = 0;
		}
	}
	if (ctok == '\n')
	{
		if (command_history == NULL)
		{
			command_history = malloc(sizeof(char *) * COMMAND_HISTORY_SIZE);
			for (int i = 0; i < COMMAND_HISTORY_SIZE; i++)
			{
				*(command_history + i) = NULL;
			}
		}
		char *command = *(command_history);
		int have_in_history = false;
		int i = 1;
		while (command != NULL)
		{
			if (strcmp(command, buffer) == 0)
			{
				have_in_history = true;
			}
			command = *(command_history + i);
			i++;
		}

		if (!have_in_history)
		{
			for (int i = 0; i < COMMAND_HISTORY_SIZE; i++)
			{
				if (*(command_history + i) == NULL)
				{
					size_t len = strlen(buffer) + 1;
					*(command_history + i) = malloc(len);
					snprintf(*(command_history + i), len, "%s", buffer);
					h_index = i;
					return count;
				}
			}
			printf("command history buffer full\n");
		}
	}
	return count;
}

int isaddop(char c)
{
	return (c == '+' || c == '-');
}
int ismulop(char c)
{
	return (c == '*' || c == '/');
}
