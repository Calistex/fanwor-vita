/* **** fed_aes.c: Auf AES-Ereignisse, Tastendruecke etc. reagieren **** */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "fedmain.h"
#include "feddefs.h"
#include "fedgraf.h"
#include "feddisk.h"
#include "sdlgui.h"

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif




/* ***Auf Tasten reagieren*** */
void handle_keypress(int key)
{
	int dx, dy;
	char *new_fname;

	switch(key)
	{
	case 'n':               /* NEWFILE */
		en_anz=0;
		for(dy=0; dy<128; dy++)
			for(dx=0; dx<128; dx++)
			{
				sfeld[dx][dy]=1;
				ffeld[dx][dy]=0;
			}
		drawoffscr(rwx, rwy, rww, rwh);
		drwindow();
		break;
	case 'l':                 /* LOADFILE */
		new_fname = SDLGui_FileSelect(fname, NULL, 0);
		if (new_fname != NULL)
		{
			strcpy(fname, new_fname);
			free(new_fname);
			loadlevel(fname);
		}
		drawoffscr(rwx, rwy, rww, rwh);
		drwindow();
		break;
	case 's':                 /* SAVEFILE: */
		new_fname = SDLGui_FileSelect(fname, NULL, 1);
		if (new_fname != NULL)
		{
			strcpy(fname, new_fname);
			free(new_fname);
			savelevel(fname);
		}
		break;
	case '0':                 /* EREASE: */
		fprintf(stderr,"\nErease mode\n");
		tmodus=0;
		acttile=0;
		break;
	case '1':                /* LVBACKG: */
		fprintf(stderr,"\nBackground setting mode\n");
		tmodus=1;
		acttile=0;
		break;
	case '2':                 /* LVEINH: */
		fprintf(stderr,"\nSprite setting mode\n");
		tmodus=2;
		acttile=0;
		break;
	case '3':                 /* LVDOORS: */
		fprintf(stderr,"\nDoor setting mode\n");
		tmodus=3;
		break;
	case '4':                   /* LVITEMS: */
		fprintf(stderr,"\nItem setting mode\n");
		tmodus=4;
		acttile=0;
		break;
	case '5':                 /* LVPEOPLE: */
		fprintf(stderr,"\n(Unsupported) People setting mode\n");
		tmodus=5;
		acttile=0;
		break;
	case 'd':                 /* LVDATEN: */
		fprintf(stderr,"\nLevel Daten:\n");
		fprintf(stderr,"Breite: %i  -  Hoehe: %i\n",r_width,r_height);
		rwx=rwy=0;
		drawoffscr(rwx, rwy, rww, rwh);
		drwindow();
		break;
	case 'h':                 /* Show help */
		printhelp();
		break;
	case 'q':                 /* QUITIT */	/* Quit program */
		endeflag=TRUE;
		break;
	}

}
