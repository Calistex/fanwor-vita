/*************************************************************/
/* fwgui.c  -  Graphical User Interface dependend functions  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>

#include "fwguiini.h"
#include "fwmain.h"
#include "fwgui.h"
#include "fwgraf.h"
#include "fwmusic.h"


/* *Variables:* */
static int gamespeed = 12;


/* ***Wait some milliseconds*** */
void waitms(short ms)
{
	SDL_Delay(ms);
}


/* ***Show an choice box*** */
/*int choicedlg(char *txt, char *buts, int defb)
{
	puts(txt);
	puts(buts);

	return( getchar()-'0' );
}*/

/* ***Show an alert box*** */
void alertdlg(char *alstr)
{
	printf("%s\n", alstr);
}

/* ***Show a fatal error message, then quit*** */
void errfatldlg(char *errstr)
{
	fprintf(stderr, "Error: %s\n", errstr);
	close_window();
	exitGUI();
	exit(-1);
}


/**
 * Called when a key has been pressed
 */
static void keyklicks(void)
{
	Uint8 *keys;

	keys = SDL_GetKeyState(NULL);
	if (keys[SDLK_ESCAPE] == SDL_PRESSED)  flag_quit=TRUE;
	tdirectn=0;
	if (keys[SDLK_UP] == SDL_PRESSED)      tdirectn=1;
	if (keys[SDLK_RIGHT] == SDL_PRESSED)   tdirectn=2;
	if (keys[SDLK_DOWN] == SDL_PRESSED)    tdirectn=3;
	if (keys[SDLK_LEFT] == SDL_PRESSED)    tdirectn=4;
	if (keys[SDLK_SPACE] == SDL_PRESSED)   tdirectn=128;
}


/**
 * Called when a button has been pressed
 */
static void buttonpress(SDL_Event event)
{
    tdirectn=0;
	if ( event.jbutton.button == GJ_UP && event.jbutton.state == SDL_PRESSED ) 
    {
        tdirectn=1;
    }
    if ( event.jbutton.button == GJ_RIGHT && event.jbutton.state == SDL_PRESSED) 
    {
        tdirectn=2;
    }
    if ( event.jbutton.button == GJ_DOWN && event.jbutton.state == SDL_PRESSED) 
    {
        tdirectn=3;
    }
    if ( event.jbutton.button == GJ_LEFT && event.jbutton.state == SDL_PRESSED) 
    {
        tdirectn=4;
    }
    if ( event.jbutton.button == GJ_CROSS && event.jbutton.state == SDL_PRESSED) 
    {
        tdirectn=128;
    }
}


/* ***Event function*** */
void event_handler(void)
{
	SDL_Event event;

	if (SDL_PollEvent(&event) == 1)
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			keyklicks();
			break;
		case SDL_KEYUP:
			/*tdirectn=0;*/
			keyklicks();
			break;
        case SDL_JOYBUTTONDOWN:
            buttonpress(event);
			break;
        case SDL_JOYBUTTONUP:
            buttonpress(event);
			break;
		case SDL_QUIT:
			flag_quit=TRUE;
			break;
		}
	}

	SDL_Delay(gamespeed);
}
