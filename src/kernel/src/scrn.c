
#include <system.h>

unsigned short *textmemptr;
int attrib = 0x0f; // 0x02;//0x1F;
int csr_x = 0;
int csr_y = 0;
int numcols = 80;
int numrows = 25;

#define TERM_NO_TERMINAL (0)
#define TERM_VT100 (1)

int terminal_type = TERM_NO_TERMINAL;

int get_num_columns()
{
	return numcols;
}
int get_num_rows()
{
	return numrows;
}
void set_num_columns(int nc)
{
	numcols = nc;
}
void set_num_rows(int nr)
{
	numrows = nr;
}
int get_cursor_x()
{
	return csr_x;
}
int get_cursor_y()
{
	return csr_y;
}
void scroll()
{
	unsigned blank, temp;
	blank = 0x20 | (attrib << 8);
	if (csr_y >= numrows)
	{
		temp = csr_y - numrows + 1;
		memcpy(textmemptr, textmemptr + temp * numcols, (numrows - temp) * numcols * 2);
		memsetw(textmemptr + (numrows - temp) * numcols, blank, numcols);
		csr_y = numrows - 1;
	}
}

void move_csr()
{
	unsigned temp;
	temp = csr_y * 80 + csr_x;
	outportb(0x3D4, 14);
	outportb(0x3D5, temp >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, temp);
}

void get_screen_size(int *x, int *y)
{
	*x = numcols;
	*y = numrows;
}
unsigned short *video_mem_ptr()
{
	return textmemptr;
}

void cls()
{
	unsigned blank;
	int i;
	blank = 0x20 | (attrib << 8);
	for (i = 0; i < numrows; i++)
		memsetw(textmemptr + i * numcols, blank, numcols);
	csr_x = 0;
	csr_y = 0;
	move_csr();
}

void getxy(int *x, int *y)
{
	*x = csr_x;
	*y = csr_y;
}
void gotoxy(int x, int y)
{
	csr_x = x;
	csr_y = y;
	move_csr();
}
unsigned short vid_read_charattrib(int x, int y)
{
	unsigned short c = *(textmemptr + (y * numcols + x));
	return c;
}
char vid_read_char(int x, int y)
{
	unsigned short vc = vid_read_charattrib(x, y);
	int c = vc & 0x00FF;
	return c;
}
unsigned char vid_read_attrib(int x, int y)
{
	unsigned short vc = vid_read_charattrib(x, y);
	unsigned int c = vc >> 8;
	return c;
}


void putch(unsigned char c)
{
	unsigned short *where;
	unsigned att = attrib << 8;
	//if(c>128)
	//	return;
	if (c == 0x08 )
	{
		if (csr_x != 0)
			csr_x--;
		where = textmemptr + (csr_y * numcols + csr_x);
		*where = ' ' | att; /* Character AND attributes: color */
	}
	else if (c == 0x09)
	{
		csr_x = (csr_x + 8) & ~(8 - 1);
	}
	else if (c == '\r')
	{
		csr_x = 0;
	}
	else if (c == '\n')
	{
		csr_x = 0;
		csr_y++;
	}
	else if (c >= ' ')
	{
		where = textmemptr + (csr_y * numcols + csr_x);
		*where = c | att; /* Character AND attributes: color */
		csr_x++;
	}
	if (csr_x >= numcols)
	{
		csr_x = 0;
		csr_y++;
	}
	scroll();
	move_csr();
}

/* Uses the above routine to output a string... */
int puts(const char *text)
{
	int i;
	int len = strlen(text);
	for (i = 0; i < len; i++)
	{
		putch(text[i]);
	}
	putch('\n');
	return len;
}

void putch_xy(int x, int y, unsigned char c)
{
	int oldx = csr_x;
	int oldy = csr_y;
	csr_x = x;
	csr_y = y;
	putch(c);
	csr_x = oldx;
	csr_y = oldy;
}

void puts_xy(int x, int y, unsigned char *text)
{
	int oldx = csr_x;
	int oldy = csr_y;
	csr_x = x;
	csr_y = y;
	int i;
	int len = strlen(text);
	for (i = 0; i < len; i++)
	{
		putch(text[i]);
	}
	csr_x = oldx;
	csr_y = oldy;
}
void set_text_attrib(unsigned char attr)
{
	attrib = attr;
}
/* Sets the forecolor and backcolor that we will use */
void set_text_colour(unsigned char forecolor, unsigned char backcolor)
{
	/* Top 4 bytes are the background, bottom 4 bytes
	 *  are the foreground color */
	attrib = (backcolor << 4) | (forecolor & 0x0F);
}
uint8_t get_foreground_colour()
{
	return (attrib << 4) >> 4;
}
uint8_t get_background_colour()
{
	return attrib >> 4;
}
void set_background_colour(unsigned char colour)
{
	uint8_t fgc = (attrib << 4) >> 4;
	attrib = (colour << 4) | (fgc & 0x0F);
}
void set_foreground_colour(unsigned char colour)
{
	uint8_t bgc = attrib >> 4;
	attrib = (bgc << 4) | (colour & 0x0F);
}
void disable_blink()
{
	inportb(0x3DA);
	outportb(0x3C0, 0x30);
	uint8_t value = inportb(0x3C1);
	outportb(0x3C0, value & 0xF7);
}
/* Sets our text-mode VGA pointer, then clears the screen for us */
void init_video(void)
{
	//disable_blink();
	textmemptr = (unsigned short *)0xB8000;
	cls();
}

int putchar(int c)
{
	putch((char)c);
	return 1;
}

void disable_cursor()
{
	outportw(0x3D4, 0x200A);
}

void enable_cursor()
{
	outportw(0x3D4, 0xE0A);
}
