#include <system.h>
#include <scheme.h>
#include "scheme-private.h"
extern int want_scheme_quit;

#define SCHEME_BUFFER_SIZE (4096)

pointer sc_exit(scheme *sc, pointer args)
{
	want_scheme_quit = true;
	return sc->NIL;
}

pointer sc_gotoxy(scheme *sc, pointer args)
{
	int x = sc->vptr->ivalue(sc->vptr->pair_car(args));
	args = sc->vptr->pair_cdr(args);
	int y = sc->vptr->ivalue(sc->vptr->pair_car(args));
	gotoxy(x, y);
	return sc->NIL;
}

pointer sc_cls(scheme *sc, pointer args)
{
	cls();
	return sc->NIL;
}

pointer sc_textcolour(scheme *sc, pointer args)
{
	char *fgcs = sc->vptr->string_value(sc->vptr->pair_car(args));
	args = sc->vptr->pair_cdr(args);
	char *bgcs = sc->vptr->string_value(sc->vptr->pair_car(args));

	int fgc = get_foreground_colour();
	int bgc = get_background_colour();

	if (!strcmp(fgcs, "VGA_BLACK"))
		fgc = VGA_BLACK;
	else if (!strcmp(fgcs, "VGA_BLUE"))
		fgc = VGA_BLUE;
	else if (!strcmp(fgcs, "VGA_GREEN"))
		fgc = VGA_GREEN;
	else if (!strcmp(fgcs, "VGA_CYAN"))
		fgc = VGA_CYAN;
	else if (!strcmp(fgcs, "VGA_RED"))
		fgc = VGA_RED;
	else if (!strcmp(fgcs, "VGA_MAGENTA"))
		fgc = VGA_MAGENTA;
	else if (!strcmp(fgcs, "VGA_BROWN"))
		fgc = VGA_BROWN;
	else if (!strcmp(fgcs, "VGA_LIGHTGREY"))
		fgc = VGA_LIGHTGREY;
	else if (!strcmp(fgcs, "VGA_DARKGREY"))
		fgc = VGA_DARKGREY;
	else if (!strcmp(fgcs, "VGA_LIGHTBLUE"))
		fgc = VGA_LIGHTBLUE;
	else if (!strcmp(fgcs, "VGA_LIGHTGREEN"))
		fgc = VGA_LIGHTGREEN;
	else if (!strcmp(fgcs, "VGA_LIGHTCYAN"))
		fgc = VGA_LIGHTCYAN;
	else if (!strcmp(fgcs, "VGA_LIGHTRED"))
		fgc = VGA_LIGHTRED;
	else if (!strcmp(fgcs, "VGA_LIGHTMAGENTA"))
		fgc = VGA_LIGHTMAGENTA;
	else if (!strcmp(fgcs, "VGA_YELLOW"))
		fgc = VGA_YELLOW;
	else if (!strcmp(fgcs, "VGA_WHITE"))
		fgc = VGA_WHITE;

	if (!strcmp(bgcs, "VGA_BLACK"))
		bgc = VGA_BLACK;
	else if (!strcmp(bgcs, "VGA_BLUE"))
		bgc = VGA_BLUE;
	else if (!strcmp(bgcs, "VGA_GREEN"))
		bgc = VGA_GREEN;
	else if (!strcmp(bgcs, "VGA_CYAN"))
		bgc = VGA_CYAN;
	else if (!strcmp(bgcs, "VGA_RED"))
		bgc = VGA_RED;
	else if (!strcmp(bgcs, "VGA_MAGENTA"))
		bgc = VGA_MAGENTA;
	else if (!strcmp(bgcs, "VGA_BROWN"))
		bgc = VGA_BROWN;
	else if (!strcmp(bgcs, "VGA_LIGHTGREY"))
		bgc = VGA_LIGHTGREY;
	else if (!strcmp(bgcs, "VGA_DARKGREY"))
		bgc = VGA_DARKGREY;
	else if (!strcmp(bgcs, "VGA_LIGHTBLUE"))
		bgc = VGA_LIGHTBLUE;
	else if (!strcmp(bgcs, "VGA_LIGHTGREEN"))
		bgc = VGA_LIGHTGREEN;
	else if (!strcmp(bgcs, "VGA_LIGHTCYAN"))
		bgc = VGA_LIGHTCYAN;
	else if (!strcmp(bgcs, "VGA_LIGHTRED"))
		bgc = VGA_LIGHTRED;
	else if (!strcmp(bgcs, "VGA_LIGHTMAGENTA"))
		bgc = VGA_LIGHTMAGENTA;
	else if (!strcmp(bgcs, "VGA_YELLOW"))
		bgc = VGA_YELLOW;
	else if (!strcmp(bgcs, "VGA_WHITE"))
		bgc = VGA_WHITE;

	set_text_colour(fgc, bgc);
	return sc->NIL;
}
pointer sc_listregistered(scheme *sc, pointer args);
scheme_registerable ff_list[] = {
	{sc_exit, "exit"},
	{sc_gotoxy, "gotoxy"},
	{sc_cls, "cls"},
	{sc_listregistered, "api"},
	{sc_textcolour, "textcolour"}};

pointer sc_listregistered(scheme *sc, pointer args)
{
	int s = sizeof(ff_list) / sizeof(scheme_registerable);
	scheme_registerable *rg = &ff_list[0];
	while (s--)
	{
		printf("%s ", rg->name);
		rg++;
	}
	printf("\n");
	return sc->NIL;
}

scheme *sc_init()
{
	int s = sizeof(ff_list) / sizeof(scheme_registerable);
	want_scheme_quit = false;
	scheme *sc;
	sc = scheme_init_new();
	scheme_set_input_port_file(sc, stdin);
	scheme_set_output_port_file(sc, stdout);
	scheme_register_foreign_func_list(sc, ff_list, s);
	return sc;
}

extern char *prompt;
extern int plen_active;
int sc_main(int argc, char **argv)
{
	int ofgc = get_foreground_colour();
	int obgc = get_background_colour();

	plen_active = false;
	sprintf(prompt, "%s", "#");
	int result = 0;
	scheme *sc = sc_init();
	char *lbuffer = malloc(SCHEME_BUFFER_SIZE);
	char *sbuffer = malloc(SCHEME_BUFFER_SIZE);
	char *obuffer = malloc(SCHEME_BUFFER_SIZE);
	char *obuffer_end = obuffer + SCHEME_BUFFER_SIZE;
	int count = 0;

	memset(sbuffer, 0, SCHEME_BUFFER_SIZE);
	memset(lbuffer, 0, SCHEME_BUFFER_SIZE);
	memset(obuffer, 0, SCHEME_BUFFER_SIZE);
	FILE *scminit = fopen("/init.scm", "r");
	if (scminit)
	{
		int len = fgetsize(scminit);
		char *buf = malloc(len + 1);
		memset(buf, 0, len + 1);
		fread(buf, 1, len, scminit);
		fclose(scminit);
		scheme_load_string(sc, buf);
		free(buf);
	}
	if (argc == 2)
	{
		FILE *fp = fopen(argv[1], "r");
		if (fp)
		{
			int len = fgetsize(fp);
			char *buf = malloc(len + 1);
			memset(buf, 0, len + 1);
			fread(buf, 1, len, fp);
			scheme_load_string(sc, buf);
			fclose(fp);
			free(buf);
			sprintf(prompt, "%s", ">");
			plen_active = true;
			scheme_deinit(sc);
			free(sbuffer);
			free(lbuffer);
			free(obuffer);
			return result;
		}
	}
	else
	{
		printf("TinyScheme 2.3.0\n");
		printf("%s", prompt);
		do
		{
			sh_getline(lbuffer, SCHEME_BUFFER_SIZE);
			if (strlen(lbuffer) > 1)
			{
				sprintf(sbuffer, "(display %s)", lbuffer);
				memset(obuffer, 0, SCHEME_BUFFER_SIZE);
				scheme_set_output_port_string(sc, obuffer, obuffer_end);
				scheme_load_string(sc, sbuffer);
				if ((strcmp(obuffer, "()") != 0)) //&& (strcmp(obuffer,"#t")!=0) && (strcmp(obuffer,"#f")!=0) )
				{
					printf("%s\n%s", obuffer, prompt);
				}
				else
				{
					if (!want_scheme_quit)
						printf("%s", prompt);
				}
				memset(sbuffer, 0, SCHEME_BUFFER_SIZE);
				memset(lbuffer, 0, SCHEME_BUFFER_SIZE);
				count++;
			}
		} while (!want_scheme_quit);
	}
	scheme_deinit(sc);
	free(sc);
	sc=NULL;
	free(sbuffer);
	free(lbuffer);
	free(obuffer);
	set_foreground_colour(ofgc);
	set_background_colour(obgc);
	sprintf(prompt, "%s", ">");
	plen_active = true;
	return result;
}
