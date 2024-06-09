/* Kilo -- A very simple editor in less than 1-kilo lines of code (as counted
 *         by "cloc"). Does not depend on libcurses, directly emits VT100
 *         escapes on the terminal.
 *
 * -----------------------------------------------------------------------
 *
 * Copyright (C) 2016 Salvatore Sanfilippo <antirez at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define KILO_VERSION "0.1.0"

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <stdarg.h>

/* Syntax highlight types */
#define HL_NORMAL 0
#define HL_NONPRINT 1
#define HL_COMMENT 2   /* Single line comment. */
#define HL_MLCOMMENT 3 /* Multi-line comment. */
#define HL_KEYWORD1 4
#define HL_KEYWORD2 5
#define HL_STRING 6
#define HL_NUMBER 7
#define HL_MATCH 8 /* Search match. */

#define HL_HIGHLIGHT_STRINGS (1 << 0)
#define HL_HIGHLIGHT_NUMBERS (1 << 1)


//void serial_puts(const char *msg);

struct editorSyntax
{
	char **filematch;
	char **keywords;
	char singleline_comment_start[2];
	char multiline_comment_start[3];
	char multiline_comment_end[3];
	int flags;
};

/* This structure represents a single line of the file we are editing. */
typedef struct erow
{
	int idx;		   /* Row index in the file, zero-based. */
	int size;		   /* Size of the row, excluding the null term. */
	int rsize;		   /* Size of the rendered row. */
	char *chars;	   /* Row content. */
	char *render;	   /* Row content "rendered" for screen (for TABs). */
	unsigned char *hl; /* Syntax highlight type for each character in render.*/
	int hl_oc;		   /* Row had open comment at end in last syntax highlight
												check. */
} erow;

typedef struct hlcolor
{
	int r, g, b;
} hlcolor;

struct editorConfig
{
	int cx, cy;		/* Cursor x and y position in characters */
	int rowoff;		/* Offset of row displayed. */
	int coloff;		/* Offset of column displayed. */
	int screenrows; /* Number of rows that we can show */
	int screencols; /* Number of cols that we can show */
	int numrows;	/* Number of rows */
	int rawmode;	/* Is terminal raw mode enabled? */
	erow *row;		/* Rows */
	int dirty;		/* File modified but not saved. */
	char *filename; /* Currently open filename */
	char statusmsg[80];
	time_t statusmsg_time;
	struct editorSyntax *syntax; /* Current syntax highlight, or NULL. */
};

static struct editorConfig E;

enum KEY_ACTION
{
	KEY_NULL = 0,  /* NULL */
	FUNC_1 = 141,  /* Ctrl-c */
	FUNC_2 = 142,  /* Ctrl-d */
	FUNC_3 = 143,  /* Ctrl-f */
	FUNC_4 = 144,  /* Ctrl-h */
	TAB = 9,	   /* Tab */
	FUNC_5 = 145,  /* Ctrl+l */
	ENTER = 10,	   /* Enter */
	FUNC_6 = 146,  /* Ctrl-q */
	FUNC_7 = 147,  /* Ctrl-s */
	FUNC_8 = 148,  /* Ctrl-u */
	ESC = 27,	   /* Escape */
	BACKSPACE = 8, /* Backspace */

	ARROW_LEFT = 128,
	ARROW_RIGHT = 129,
	ARROW_UP = 130,
	ARROW_DOWN = 140,
	DEL_KEY = 156,
	HOME_KEY = 159,
	END_KEY = 160,
	PAGE_UP = 153,
	PAGE_DOWN = 154,
	INSERT_KEY = 155
};

void editorProcessKeypress();
int editorReadKey();
void editorSetStatusMessage(const char *msg);
void editorMoveCursor(int key);

/* =========================== Syntax highlights DB =========================
 *
 * In order to add a new syntax, define two arrays with a list of file name
 * matches and keywords. The file name matches are used in order to match
 * a given syntax with a given file name: if a match pattern starts with a
 * dot, it is matched as the last past of the filename, for example ".c".
 * Otherwise the pattern is just searched inside the filenme, like "Makefile").
 *
 * The list of keywords to highlight is just a list of words, however if they
 * a trailing '|' character is added at the end, they are highlighted in
 * a different color, so that you can have two different sets of keywords.
 *
 * Finally add a stanza in the HLDB global variable with two two arrays
 * of strings, and a set of flags in order to enable highlighting of
 * comments and numbers.
 *
 * The characters for single and multi line comments must be exactly two
 * and must be provided as well (see the C language example).
 *
 * There is no support to highlight patterns currently. */

/* C / C++ */
char *C_HL_extensions[] = {".c", ".h", ".cpp", ".hpp", ".cc", NULL};
char *C_HL_keywords[] = {
	/* C Keywords */
	"auto", "break", "case", "continue", "default", "do", "else", "enum",
	"extern", "for", "goto", "if", "register", "return", "sizeof", "static",
	"struct", "switch", "typedef", "union", "volatile", "while", "NULL",

	/* C++ Keywords */
	"alignas", "alignof", "and", "and_eq", "asm", "bitand", "bitor", "class",
	"compl", "constexpr", "const_cast", "deltype", "delete", "dynamic_cast",
	"explicit", "export", "false", "friend", "inline", "mutable", "namespace",
	"new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq",
	"private", "protected", "public", "reinterpret_cast", "static_assert",
	"static_cast", "template", "this", "thread_local", "throw", "true", "try",
	"typeid", "typename", "virtual", "xor", "xor_eq",

	/* C types */
	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
	"void|", "short|", "auto|", "const|", "bool|", NULL};

/* Here we define an array of syntax highlights by extensions, keywords,
 * comments delimiters and flags. */
struct editorSyntax HLDB[] =
	{
		{/* C / C++ */
		 C_HL_extensions,
		 C_HL_keywords,
		 "//", "/*", "*/",
		 HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS}};
char *ASM_HL_extensions[] = {".asm", ".inc", ".s", NULL};
char *ASM_HL_keywords[] = {
	/* ASM Keywords */
	"mov", "add", "sub", "mul", "div", "call", "ret",
	"and", "or", "xor", "not", "rep", "loop",
	"jmp", "jne", "jlt", "push", "pop", "pusha", "popa",
	"cmp", "je", "jle", "jge", "jg", "jnz",
	"hlt", "cli", "sti",
	"eax|", "ebx|", "ecx|", "edx|", "esi|", "edi|", "esp|", "ebp|",
	"ax|", "bx|", "cx|", "dx|", "si|", "di|", "sp|", "bp|",
	"al|", "ah|", "bl|", "bh|", "cl|", "ch|", "dl|", "dh|",

	"cs|", "ds|", "es|", "fs|", "gs|", "cr0|", "cr1|", "cr2|", "cr3|", "cr4|",
	"byte", "word", "dword", "qword", "macro", "proc", "endp", "int",
	"use32", "use64", "include", "format", "align", "org", "elf", "section",
	"executable", "readable", "writeable", "public", "ccall","extrn","align","use32","format",

	/* ASM Keywords2 */
	//"executable|","readable|","writeable|","public|","extrn|",
	//"section|","segment|","align|","use32|","format|","elf|",
	/* ASM Types */
	"db|", "dd|", "resb|", "resd|", "file|", "rb|", "dw|", "du|", "rw|",
	"dd|", "df|", "dp|", "rd|", "rp|", "rf|", "dq|", "rq|", "dt|", "rt|"};
char *ASM_HL_keywords2[] = {
	"executable", "readable", "writeable"};

struct editorSyntax ASM_HLDB[] =
	{
		{ASM_HL_extensions,
		 ASM_HL_keywords,
		 ";;", "/*", "*/",
		 HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS}};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))
#define ASM_HLDB_ENTRIES (sizeof(ASM_HLDB) / sizeof(ASM_HLDB[0]))

int kilo_quit = false;
int synhl = true;
int showlinenum = false;
/* ======================= Low level terminal handling ====================== */

/* ====================== Syntax highlight color scheme  ==================== */

int is_separator(int c)
{
	return c == '\0' || isspace(c) || strchr(",.()+-/*=~%[];", c) != NULL;
}

/* Return true if the specified row last char is part of a multi line comment
 * that starts at this row or at one before, and does not end at the end
 * of the row but spawns to the next row. */
int editorRowHasOpenComment(erow *row)
{
	if (row->hl && row->rsize && row->hl[row->rsize - 1] == HL_MLCOMMENT &&
		(row->rsize < 2 || (row->render[row->rsize - 2] != '*' ||
							row->render[row->rsize - 1] != '/')))
		return 1;
	return 0;
}

/* Set every byte of row->hl (that corresponds to every character in the line)
 * to the right syntax highlight type (HL_* defines). */
void editorUpdateSyntax(erow *row)
{
	row->hl = realloc(row->hl, row->rsize);
	memset(row->hl, HL_NORMAL, row->rsize);

	if (E.syntax == NULL)
		return; /* No syntax, everything is HL_NORMAL. */

	int i, prev_sep, in_string, in_comment;
	char *p;
	char **keywords = E.syntax->keywords;
	char *scs = E.syntax->singleline_comment_start;
	char *mcs = E.syntax->multiline_comment_start;
	char *mce = E.syntax->multiline_comment_end;

	/* Point to the first non-space char. */
	p = row->render;
	i = 0; /* Current char offset */
	while (*p && isspace(*p))
	{
		p++;
		i++;
	}
	prev_sep = 1;	/* Tell the parser if 'i' points to start of word. */
	in_string = 0;	/* Are we inside "" or '' ? */
	in_comment = 0; /* Are we inside multi-line comment? */

	/* If the previous line has an open comment, this line starts
	 * with an open comment state. */
	if (row->idx > 0 && editorRowHasOpenComment(&E.row[row->idx - 1]))
		in_comment = 1;

	while (*p)
	{
		/* Handle // comments. */
		if (prev_sep && *p == scs[0] && *(p + 1) == scs[1])
		{
			/* From here to end is a comment */
			memset(row->hl + i, HL_COMMENT, row->size - i);
			return;
		}

		/* Handle multi line comments. */
		if (in_comment)
		{
			row->hl[i] = HL_MLCOMMENT;
			if (*p == mce[0] && *(p + 1) == mce[1])
			{
				row->hl[i + 1] = HL_MLCOMMENT;
				p += 2;
				i += 2;
				in_comment = 0;
				prev_sep = 1;
				continue;
			}
			else
			{
				prev_sep = 0;
				p++;
				i++;
				continue;
			}
		}
		else if (*p == mcs[0] && *(p + 1) == mcs[1])
		{
			row->hl[i] = HL_MLCOMMENT;
			row->hl[i + 1] = HL_MLCOMMENT;
			p += 2;
			i += 2;
			in_comment = 1;
			prev_sep = 0;
			continue;
		}

		/* Handle "" and '' */
		if (in_string)
		{
			row->hl[i] = HL_STRING;
			if (*p == '\\')
			{
				row->hl[i + 1] = HL_STRING;
				p += 2;
				i += 2;
				prev_sep = 0;
				continue;
			}
			if (*p == in_string)
				in_string = 0;
			p++;
			i++;
			continue;
		}
		else
		{
			if (*p == '"' || *p == '\'')
			{
				in_string = *p;
				row->hl[i] = HL_STRING;
				p++;
				i++;
				prev_sep = 0;
				continue;
			}
		}

		/* Handle non printable chars. */
		if (!isprint(*p))
		{
			row->hl[i] = HL_NONPRINT;
			p++;
			i++;
			prev_sep = 0;
			continue;
		}

		/* Handle numbers */
		if ((isdigit(*p) && (prev_sep || row->hl[i - 1] == HL_NUMBER)) ||
			(*p == '.' && i > 0 && row->hl[i - 1] == HL_NUMBER) ||
			(toupper(*p) == 'H' && i > 0 && row->hl[i - 1] == HL_NUMBER) ||
			((toupper(*p) >= 'A' && toupper(*p) <= 'F') && i > 0 && row->hl[i - 1] == HL_NUMBER)

		)
		{
			row->hl[i] = HL_NUMBER;
			p++;
			i++;
			prev_sep = 0;
			continue;
		}

		/* Handle keywords and lib calls */
		if (prev_sep)
		{
			int j;
			for (j = 0; keywords[j]; j++)
			{
				int klen = strlen(keywords[j]);
				int kw2 = keywords[j][klen - 1] == '|';

				if (kw2)
					klen--;

				if (!memcmp(p, keywords[j], klen) &&
					is_separator(*(p + klen)))
				{
					/* Keyword */
					memset(row->hl + i, kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
					p += klen;
					i += klen;
					break;
				}
			}
			if (keywords[j] != NULL)
			{
				prev_sep = 0;
				continue; /* We had a keyword match */
			}
		}

		/* Not special chars */
		prev_sep = is_separator(*p);
		p++;
		i++;
	}

	/* Propagate syntax change to the next row if the open commen
	 * state changed. This may recursively affect all the following rows
	 * in the file. */
	int oc = editorRowHasOpenComment(row);
	if (row->hl_oc != oc && row->idx + 1 < E.numrows)
		editorUpdateSyntax(&E.row[row->idx + 1]);
	row->hl_oc = oc;
}

/* Maps syntax highlight token types to terminal colors. */
int editorSyntaxToColor(int hl)
{
	switch (hl)
	{
	case HL_COMMENT:
	case HL_MLCOMMENT:
		return VGA_LIGHTGREY;
	case HL_KEYWORD1:
		return VGA_YELLOW;
	case HL_KEYWORD2:
		return VGA_GREEN;
	case HL_STRING:
		return VGA_LIGHTBLUE;
	case HL_NUMBER:
		return VGA_LIGHTBLUE;
	case HL_MATCH:
		return VGA_BLUE;
	default:
		return VGA_WHITE;
	}
}

/* Select the syntax highlight scheme depending on the filename,
 * setting it in the global state E.syntax. */
void editorSelectSyntaxHighlight(char *filename)
{
	for (unsigned j = 0; j < HLDB_ENTRIES; j++)
	{
		struct editorSyntax *s = HLDB + j;
		unsigned int i = 0;
		while (s->filematch[i])
		{
			char *p;
			int patlen = strlen(s->filematch[i]);
			if ((p = strstr(filename, s->filematch[i])) != NULL)
			{
				if (s->filematch[i][0] != '.' || p[patlen] == '\0')
				{
					E.syntax = s;
					return;
				}
			}
			i++;
		}
	}

	for (unsigned j = 0; j < ASM_HLDB_ENTRIES; j++)
	{
		struct editorSyntax *s = ASM_HLDB + j;
		unsigned int i = 0;
		while (s->filematch[i])
		{
			char *p;
			int patlen = strlen(s->filematch[i]);
			if ((p = strstr(filename, s->filematch[i])) != NULL)
			{
				if (s->filematch[i][0] != '.' || p[patlen] == '\0')
				{
					E.syntax = s;
					return;
				}
			}
			i++;
		}
	}
}

/* ======================= Editor rows implementation ======================= */

/* Update the rendered version and the syntax highlight of a row. */
void editorUpdateRow(erow *row)
{
	unsigned int tabs = 0, nonprint = 0;
	int j, idx;

	/* Create a version of the row we can directly print on the screen,
	 * respecting tabs, substituting non printable characters with '?'. */
	if (row->render)
		free(row->render);

	for (j = 0; j < row->size; j++)
	{
		if (row->chars[j] == TAB)
		{
			tabs++;
		}
	}

	unsigned long long allocsize =
		(unsigned long long)row->size + tabs * 8 + nonprint * 9 + 1;
	if (allocsize > ULONG_MAX)
	{
		cls();
		printf("Some line of the edited file is too long for kilo\n");
		getchar();
		return;
	}
	row->render = malloc(row->size + tabs * 8 + nonprint * 9 + 1);
	idx = 0;
	for (j = 0; j < row->size; j++)
	{
		if (row->chars[j] == TAB)
		{
			row->render[idx++] = ' ';
			while ((idx + 1) % 8 != 0)
				row->render[idx++] = ' ';
		}
		else
		{
			row->render[idx++] = row->chars[j];
		}
	}
	row->rsize = idx;
	row->render[idx] = '\0';

	/* Update the syntax highlighting attributes of the row. */
	editorUpdateSyntax(row);
}

/* Insert a row at the specified position, shifting the other rows on the bottom
 * if required. */
void editorInsertRow(int at, char *s, size_t len)
{
	if (at > E.numrows)
		return;
	E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
	if (at != E.numrows)
	{
		memmove(E.row + at + 1, E.row + at, sizeof(E.row[0]) * (E.numrows - at));
		int j;
		for (j = at + 1; j <= E.numrows; j++)
			E.row[j].idx++;
	}
	E.row[at].size = len;
	E.row[at].chars = malloc(len + 1);
	memcpy(E.row[at].chars, s, len + 1);
	E.row[at].hl = NULL;
	E.row[at].hl_oc = 0;
	E.row[at].render = NULL;
	E.row[at].rsize = 0;
	E.row[at].idx = at;
	editorUpdateRow(E.row + at);
	E.numrows++;
	E.dirty++;
}

/* Free row's heap allocated stuff. */
void editorFreeRow(erow *row)
{
	free(row->render);
	free(row->chars);
	free(row->hl);
}

/* Remove the row at the specified position, shifting the remainign on the
 * top. */
void editorDelRow(int at)
{
	erow *row;

	if (at >= E.numrows)
		return;
	row = E.row + at;
	editorFreeRow(row);
	memmove(E.row + at, E.row + at + 1, sizeof(E.row[0]) * (E.numrows - at - 1));
	int j;
	for (j = at; j < E.numrows - 1; j++)
		E.row[j].idx++;
	E.numrows--;
	E.dirty++;
}

/* Turn the editor rows into a single heap-allocated string.
 * Returns the pointer to the heap-allocated string and populate the
 * integer pointed by 'buflen' with the size of the string, escluding
 * the final nulterm. */
char *editorRowsToString(int *buflen)
{
	char *buf = NULL, *p;
	int totlen = 0;
	int j;

	/* Compute count of bytes */
	for (j = 0; j < E.numrows; j++)
		totlen += E.row[j].size + 1; /* +1 is for "\n" at end of every row */
	*buflen = totlen;
	totlen++; /* Also make space for nulterm */

	p = buf = malloc(totlen);
	for (j = 0; j < E.numrows; j++)
	{
		memcpy(p, E.row[j].chars, E.row[j].size);
		p += E.row[j].size;
		*p = '\n';
		p++;
	}
	*p = '\0';
	return buf;
}

/* Insert a character at the specified position in a row, moving the remaining
 * chars on the right if needed. */
void editorRowInsertChar(erow *row, int at, int c)
{
	if (at > row->size)
	{
		/* Pad the string with spaces if the insert location is outside the
		 * current length by more than a single character. */
		int padlen = at - row->size;
		/* In the next line +2 means: new char and null term. */
		row->chars = realloc(row->chars, row->size + padlen + 2);
		memset(row->chars + row->size, ' ', padlen);
		row->chars[row->size + padlen + 1] = '\0';
		row->size += padlen + 1;
	}
	else
	{
		/* If we are in the middle of the string just make space for 1 new
		 * char plus the (already existing) null term. */
		row->chars = realloc(row->chars, row->size + 2);
		memmove(row->chars + at + 1, row->chars + at, row->size - at + 1);
		row->size++;
	}
	row->chars[at] = c;
	editorUpdateRow(row);
	E.dirty++;
}

/* Append the string 's' at the end of a row */
void editorRowAppendString(erow *row, char *s, size_t len)
{
	row->chars = realloc(row->chars, row->size + len + 1);
	memcpy(row->chars + row->size, s, len);
	row->size += len;
	row->chars[row->size] = '\0';
	editorUpdateRow(row);
	E.dirty++;
}

/* Delete the character at offset 'at' from the specified row. */
void editorRowDelChar(erow *row, int at)
{
	if (row->size <= at)
		return;
	memmove(row->chars + at, row->chars + at + 1, row->size - at);
	editorUpdateRow(row);
	row->size--;
	E.dirty++;
}

/* Insert the specified char at the current prompt position. */
void editorInsertChar(int c)
{
	int filerow = E.rowoff + E.cy;
	int filecol = E.coloff + E.cx;
	erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

	/* If the row where the cursor is currently located does not exist in our
	 * logical representaion of the file, add enough empty rows as needed. */
	if (!row)
	{
		while (E.numrows <= filerow)
			editorInsertRow(E.numrows, "", 0);
	}
	row = &E.row[filerow];
	editorRowInsertChar(row, filecol, c);
	if (E.cx == E.screencols - 1)
		E.coloff++;
	else
		E.cx++;
	E.dirty++;
}

/* Inserting a newline is slightly complex as we have to handle inserting a
 * newline in the middle of a line, splitting the line as needed. */
void editorInsertNewline()
{
	int filerow = E.rowoff + E.cy;
	int filecol = E.coloff + E.cx;
	erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

	if (!row)
	{
		if (filerow == E.numrows)
		{
			editorInsertRow(filerow, "", 0);
			goto fixcursor;
		}
		return;
	}
	/* If the cursor is over the current line size, we want to conceptually
	 * think it's just over the last character. */
	if (filecol >= row->size)
		filecol = row->size;
	if (filecol == 0)
	{
		editorInsertRow(filerow, "", 0);
	}
	else
	{
		/* We are in the middle of a line. Split it between two rows. */
		editorInsertRow(filerow + 1, row->chars + filecol, row->size - filecol);
		row = &E.row[filerow];
		row->chars[filecol] = '\0';
		row->size = filecol;
		editorUpdateRow(row);
	}
fixcursor:
	if (E.cy == E.screenrows - 1)
	{
		E.rowoff++;
	}
	else
	{
		E.cy++;
	}
	E.cx = 0;
	E.coloff = 0;
}

/* Delete the char at the current prompt position. */
void editorDelChar()
{
	int filerow = E.rowoff + E.cy;
	int filecol = E.coloff + E.cx;
	erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

	if (!row)
		editorMoveCursor(ARROW_LEFT);
	if (!row || (filecol == 0 && filerow == 0))
		return;
	if (filecol == 0)
	{
		/* Handle the case of column 0, we need to move the current line
		 * on the right of the previous one. */
		filecol = E.row[filerow - 1].size;
		editorRowAppendString(&E.row[filerow - 1], row->chars, row->size);
		editorDelRow(filerow);
		row = NULL;
		if (E.cy == 0)
			E.rowoff--;
		else
			E.cy--;
		E.cx = filecol;
		if (E.cx >= E.screencols)
		{
			int shift = (E.screencols - E.cx) + 1;
			E.cx -= shift;
			E.coloff += shift;
		}
	}
	else
	{
		editorRowDelChar(row, filecol - 1);
		if (E.cx == 0 && E.coloff)
			E.coloff--;
		else
			E.cx--;
	}
	if (row)
		editorUpdateRow(row);
	E.dirty++;
}

/* Load the specified program in the editor memory and returns 0 on success
 * or 1 on error. */
int editorOpen(char *filename)
{
	FILE *fp;
	E.dirty = 0;
	if (E.filename)
		free(E.filename);
	size_t fnlen = strlen(filename) + 1;
	E.filename = malloc(fnlen);
	memcpy(E.filename, filename, fnlen);
	fp = fopen(filename, "r");
	if (!fp)
	{
		return 0;
	}
	char *line = NULL;
	size_t linecap = 0;
	size_t linelen;

	int count = 0;
	while ((linelen = getline(&line, &linecap, fp)) != -1)
	{
		if (linelen && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
		{
			line[--linelen] = '\0';
		}

		editorInsertRow(E.numrows, line, linelen);
	}
	if (line)
		free(line);
	fclose(fp);
	E.dirty = 0;
	return 0;
}

/* Save the current file on disk. Return 0 on success, 1 on error. */
int editorSave()
{
	int len;
	char *buf = editorRowsToString(&len);
	FILE *fp = fopen(E.filename, "w");
	if (fp)
	{
		fwrite(buf, 1, len, fp);
		fclose(fp);
	}
	free(buf);
	E.dirty = 0;
	sprintf(E.statusmsg, "%d bytes written to disc", len);
	return 0;
}

/* ============================= Screen update ============================ */

struct abuf
{
	unsigned short *b;
	int len;
};

#define ABUF_INIT \
	{             \
		NULL, 0   \
	}

int abCountTabs(const char *s)
{
	int numtabs = 0;
	const char *ptr = s;
	while (*ptr != '\0')
	{
		if (*ptr == '\t')
		{
			numtabs++;
		}
		ptr++;
	}
	return numtabs;
}
void abAppend(struct abuf *ab, const char *s, int len, int fg_colour, int bg_colour)
{
	int count = 0;
	int attrib = ((bg_colour << 4) | (fg_colour & 0x0f)) << 8;
	unsigned short *n = realloc(ab->b, ab->len + (len * sizeof(unsigned short)));
	if (n == NULL)
	{
		//serial_puts("null n\r\n");
		return;
	}

	const char *ptr = s;
	unsigned short *dst = (n + (ab->len / sizeof(unsigned short)));
	while (*ptr != 0 && count < len)
	{
		*dst = *ptr | attrib;
		dst++;
		ptr++;
		count++;
	}
	ab->b = n;
	ab->len = ab->len + (len * sizeof(unsigned short));
}

void abFree(struct abuf *ab)
{
	free(ab->b);
}

void editorRefreshScreen()
{
	struct abuf ab = ABUF_INIT;
	int y;
	erow *r;
	int linenumlen = 4;
	int sw=get_num_columns();
	for (y = 0; y < E.screenrows; y++)
	{
		int filerow = E.rowoff + y;
		
		if (filerow >= E.numrows)
		{
			if (E.numrows == 0 && y == E.screenrows / 3)
			{
				char welcome[90];
				int welcomelen;
				sprintf(welcome, "Kilo Editor -- version %s", KILO_VERSION);
				welcomelen = strlen(welcome);
				int padding = (E.screencols - welcomelen) / 2;
				int epos = padding + welcomelen;
				if (padding)
				{
					abAppend(&ab, "~", 1, VGA_DARKGREY, VGA_BLACK);
					padding--;
				}
				while (padding--)
				{
					abAppend(&ab, " ", 1, VGA_DARKGREY, VGA_BLACK);
				}
				abAppend(&ab, welcome, welcomelen, VGA_WHITE, VGA_BLACK);
				while (epos < sw)
				{
					abAppend(&ab, " ", 1, VGA_DARKGREY, VGA_BLACK);
					epos++;
				}
			}
			else
			{
				int padding = sw-1;
				abAppend(&ab, "~", 1, VGA_DARKGREY, VGA_BLACK);
				while (padding--)
				{
					abAppend(&ab, " ", 1, VGA_DARKGREY, VGA_BLACK);
				}
			}
			continue;
		}

		r = &E.row[filerow];
		int len = r->rsize - E.coloff;
		int current_color = VGA_WHITE;

		int linebgcolour = VGA_BLACK;
		int linefgcolour = VGA_BLUE;

		if (showlinenum)
		{
			char buf[32];

			// if(filerow<99)
			//	linenumlen=2;
			// else if(filerow>99 && filerow<999)
			//	linenumlen=3;
			// else if(filerow>999 && filerow <9999)
			//	linenumlen=4;
			if (filerow > 9999 && filerow < 99999)
				linenumlen = 5;

			if (linenumlen == 2)
				sprintf(buf, "%02d", filerow);
			else if (linenumlen == 3)
				sprintf(buf, "%03d", filerow);
			else if (linenumlen == 4)
				sprintf(buf, "%04d", filerow);
			else if (linenumlen == 5)
				sprintf(buf, "%05d", filerow);

			abAppend(&ab, buf, strlen(buf), linefgcolour, linebgcolour);
		}
		if (len > 0 && synhl)
		{
			if (showlinenum)
			{
				if (len > E.screencols - linenumlen)
					len = E.screencols - linenumlen;
			}
			else
			{
				if (len > E.screencols)
					len = E.screencols;
			}
			char *c = r->render + E.coloff;
			unsigned char *hl = r->hl + E.coloff;
			for (int j = 0; j < len; j++)
			{
				if (hl[j] == HL_NONPRINT)
				{
					current_color = VGA_WHITE;
					abAppend(&ab, c + j, 1, current_color, VGA_BLACK);
				}
				else if (hl[j] == HL_NORMAL)
				{
					current_color = VGA_WHITE;
					abAppend(&ab, c + j, 1, current_color, VGA_BLACK);
				}
				else
				{
					int color = editorSyntaxToColor(hl[j]);
					if (color != current_color)
					{
						current_color = color;
					}
					abAppend(&ab, c + j, 1, current_color, VGA_BLACK);
				}
			}
			int l = len;
			int lm = sw;
			if (showlinenum)
				lm = lm - linenumlen;
			while (l < lm)
			{
				abAppend(&ab, " ", 1, VGA_LIGHTGREY, VGA_BLACK);
				l++;
			}
		}
		else
		{
			abAppend(&ab, r->render, len, VGA_LIGHTGREY, VGA_BLACK);
			int lm = sw;
			if (showlinenum)
				lm = lm - linenumlen;
			while (len < lm)
			{
				abAppend(&ab, " ", 1, VGA_LIGHTGREY, VGA_BLACK);
				len++;
			}
		}
	}

	//
	// STATUS BAR
	//
	char *sbuffer = malloc(1024);
	sprintf(sbuffer, "%s - %d lines", E.filename, E.numrows);
	int slen = strlen(sbuffer);
	abAppend(&ab, sbuffer, slen, VGA_DARKGREY, VGA_WHITE);
	while (slen < sw)
	{
		abAppend(&ab, " ", 1, VGA_DARKGREY, VGA_WHITE);
		slen++;
	}

	sprintf(sbuffer, "%s", E.statusmsg);
	slen = strlen(sbuffer);
	abAppend(&ab, sbuffer, slen, VGA_WHITE, VGA_BLACK);
	while (slen < sw)
	{
		abAppend(&ab, " ", 1, VGA_WHITE, VGA_BLACK);
		slen++;
	}

	unsigned short *vmem = video_mem_ptr();
	memcpy(vmem, ab.b, ab.len);
	free(sbuffer);
	abFree(&ab);
	if (showlinenum)
		gotoxy(E.cx + linenumlen, E.cy);
	else
		gotoxy(E.cx, E.cy);
}

void editorSetStatusMessage(const char *msg)
{
	sprintf(E.statusmsg, "%s", msg);
	E.statusmsg_time = 0;
}

/* =============================== Find mode ================================ */

#define KILO_QUERY_LEN 256

void editorFind()
{
	char smsgbuf[80];
	char query[KILO_QUERY_LEN + 1] = {0};
	int qlen = 0;
	int last_match = -1;	/* Last line where a match was found. -1 for none. */
	int find_next = 0;		/* if 1 search next, if -1 search prev. */
	int saved_hl_line = -1; /* No saved HL */
	char *saved_hl = NULL;

#define FIND_RESTORE_HL                                                            \
	do                                                                             \
	{                                                                              \
		if (saved_hl)                                                              \
		{                                                                          \
			memcpy(E.row[saved_hl_line].hl, saved_hl, E.row[saved_hl_line].rsize); \
			free(saved_hl);                                                        \
			saved_hl = NULL;                                                       \
		}                                                                          \
	} while (0)

	/* Save the cursor position in order to restore it later. */
	int saved_cx = E.cx, saved_cy = E.cy;
	int saved_coloff = E.coloff, saved_rowoff = E.rowoff;

	while (1)
	{
		sprintf(smsgbuf, "Search: %s (Use ESC/Arrows/Enter)", query);
		editorSetStatusMessage(smsgbuf);
		editorRefreshScreen();
		int c = editorReadKey();
		if (c == DEL_KEY || c == BACKSPACE)
		{
			if (qlen != 0)
				query[--qlen] = '\0';
			last_match = -1;
		}
		else if (c == ESC || c == ENTER)
		{
			if (c == ESC)
			{
				E.cx = saved_cx;
				E.cy = saved_cy;
				E.coloff = saved_coloff;
				E.rowoff = saved_rowoff;
			}
			FIND_RESTORE_HL;
			sprintf(smsgbuf, "%s", "HELP: F2=save|F3=search|F5=syntax|F6=linenum|Esc=quit");
			editorSetStatusMessage(smsgbuf);
			return;
		}
		else if (c == ARROW_RIGHT || c == ARROW_DOWN)
		{
			find_next = 1;
		}
		else if (c == ARROW_LEFT || c == ARROW_UP)
		{
			find_next = -1;
		}
		else if (isprint(c))
		{
			if (qlen < KILO_QUERY_LEN)
			{
				query[qlen++] = c;
				query[qlen] = '\0';
				last_match = -1;
			}
		}
		/* Search occurrence. */
		if (last_match == -1)
			find_next = 1;
		if (find_next)
		{
			char *match = NULL;
			int match_offset = 0;
			int i, current = last_match;

			for (i = 0; i < E.numrows; i++)
			{
				current += find_next;
				if (current == -1)
					current = E.numrows - 1;
				else if (current == E.numrows)
					current = 0;
				match = strstr(E.row[current].render, query);
				if (match)
				{
					match_offset = match - E.row[current].render;
					break;
				}
			}
			find_next = 0;

			/* Highlight */
			FIND_RESTORE_HL;
			if (match)
			{
				erow *row = &E.row[current];
				last_match = current;
				if (row->hl)
				{
					saved_hl_line = current;
					saved_hl = malloc(row->rsize);
					memcpy(saved_hl, row->hl, row->rsize);
					memset(row->hl + match_offset, HL_MATCH, qlen);
				}
				E.cy = 0;
				E.cx = match_offset;
				E.rowoff = current;
				E.coloff = 0;
				/* Scroll horizontally as needed. */
				if (E.cx > E.screencols)
				{
					int diff = E.cx - E.screencols;
					E.cx -= diff;
					E.coloff += diff;
				}
			}
		}
	}
	sprintf(smsgbuf, "%s", "HELP: F2=save|F3=search|F5=syntax|F6=linenum|Esc=quit");
	editorSetStatusMessage(smsgbuf);
}

/* ========================= Editor events handling  ======================== */

/* Handle cursor position change because arrow keys were pressed. */
void editorMoveCursor(int key)
{
	int filerow = E.rowoff + E.cy;
	int filecol = E.coloff + E.cx;
	int rowlen;
	erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];

	switch (key)
	{
	case ARROW_LEFT:
		if (E.cx == 0)
		{
			if (E.coloff)
			{
				E.coloff--;
			}
			else
			{
				if (filerow > 0)
				{
					E.cy--;
					E.cx = E.row[filerow - 1].size;
					if (E.cx > E.screencols - 1)
					{
						E.coloff = E.cx - E.screencols + 1;
						E.cx = E.screencols - 1;
					}
				}
			}
		}
		else
		{
			E.cx -= 1;
		}
		break;
	case ARROW_RIGHT:
		if (row && filecol < row->size)
		{
			if (E.cx == E.screencols - 1)
			{
				E.coloff++;
			}
			else
			{
				E.cx += 1;
			}
		}
		else if (row && filecol == row->size)
		{
			E.cx = 0;
			E.coloff = 0;
			if (E.cy == E.screenrows - 1)
			{
				E.rowoff++;
			}
			else
			{
				E.cy += 1;
			}
		}
		break;
	case ARROW_UP:
		if (E.cy == 0)
		{
			if (E.rowoff)
				E.rowoff--;
		}
		else
		{
			E.cy -= 1;
		}
		break;
	case ARROW_DOWN:
		if (filerow < E.numrows)
		{
			if (E.cy == E.screenrows - 1)
			{
				E.rowoff++;
			}
			else
			{
				E.cy += 1;
			}
		}
		break;
	}
	/* Fix cx if the current line has not enough chars. */
	filerow = E.rowoff + E.cy;
	filecol = E.coloff + E.cx;
	row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
	rowlen = row ? row->size : 0;
	if (filecol > rowlen)
	{
		E.cx -= filecol - rowlen;
		if (E.cx < 0)
		{
			E.coloff += E.cx;
			E.cx = 0;
		}
	}
}

int editorReadKey()
{
	return getchar();
}

void editorProcessKeypress()
{

	int c = 0;
	while ((c = kbhit()) == 0)
	{
	}
	c = editorReadKey();
	if (c == ESC)
	{
		kilo_quit = true;
		return;
	}
	switch (c)
	{
	case ENTER:
		editorInsertNewline();
		break;
	case FUNC_2:
		editorSave();
		break;
	case FUNC_3:
		editorFind();
		break;
	case FUNC_5:
		synhl = !synhl;
		break;
	case FUNC_6:
		showlinenum = !showlinenum;
		break;
	case FUNC_7:
	{
		editorDelRow(E.rowoff + E.cy);
	}
	break;
	case END_KEY:
	{
		int filerow = E.rowoff + E.cy;
		int filecol = E.coloff + E.cx;
		erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
		if (row)
		{
			int times = row->size - filecol;
			while (times--)
			{
				editorMoveCursor(ARROW_RIGHT);
			}
		}
	}
	break;
	case HOME_KEY:
	{
		int times = E.coloff + E.cx;
		while (times--)
		{
			editorMoveCursor(ARROW_LEFT);
		}
	}
	break;
	case INSERT_KEY:
		break;
	case BACKSPACE:
		editorDelChar();
		break;
	case DEL_KEY:
		editorMoveCursor(ARROW_RIGHT);
		editorDelChar();
		break;
	case PAGE_UP:
	case PAGE_DOWN:
		if (c == PAGE_UP && E.cy != 0)
			E.cy = 0;
		else if (c == PAGE_DOWN && E.cy != E.screenrows - 1)
			E.cy = E.screenrows - 1;
		{
			int times = E.screenrows;
			while (times--)
				editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
		}
		break;

	case ARROW_UP:
	case ARROW_DOWN:
	case ARROW_LEFT:
	case ARROW_RIGHT:
		editorMoveCursor(c);
		break;
	case TAB:
	{
		c = ' ';
		for (int i = 0; i < 4; i++)
		{
			editorInsertChar(c);
		}
	}
	break;
	default:
		editorInsertChar(c);
		break;
	}
}

int editorFileWasModified()
{
	return E.dirty;
}

void editorInit()
{
	E.cx = 0;
	E.cy = 0;
	E.rowoff = 0;
	E.coloff = 0;
	E.numrows = 0;
	E.row = NULL;
	E.dirty = 0;
	E.filename = NULL;
	E.syntax = NULL;
	E.screenrows = get_num_rows() - 2;
	E.screencols = get_num_columns();
	cls();
}

int kilo_main(int argc, char **argv)
{
	char smsgbuf[80];
	kilo_quit = false;
	if (argc != 2)
	{
		printf("Usage: ed <filename>\n");
		return 1;
	}
	cls();
	editorInit();
	editorSelectSyntaxHighlight(argv[1]);
	editorOpen(argv[1]);
	sprintf(smsgbuf, "%s", "HELP: F2=save|F3=search|F5=syntax|F6=linenum|Esc=quit");
	editorSetStatusMessage(smsgbuf);
	while (!kilo_quit)
	{
		editorRefreshScreen();
		editorProcessKeypress();
	}
	cls();
	//	printf("num rows=%d\n",E.numrows);
	while (E.numrows)
	{
		int i = E.numrows - 1;
		erow *row = (erow *)&E.row[i];
		if (row)
		{
			// printf("free row|");
			editorDelRow(i);
		}
	}
	// printf("\nnum rows=%d\n",E.numrows);
	return 0;
}
