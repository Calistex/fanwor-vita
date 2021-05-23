/* *** fwgraf.h - Variables and prototypes of fwgraf.c *** */
/* *** Written and (c) by Thomas Huth, 1999 *** */

#ifndef _FWGRAF_H
#define _FWGRAF_H

#include <SDL/SDL.h>


void drawsprites(void);
void clearsprite(short nr);
void drawblock(short xpos, short ypos, short num);
void offscr2win(short xpos, short ypos, short width, short height);
void write_hp_gold(void);
int showpicture(char *name);

extern SDL_Surface *sdlscrn;
extern SDL_Surface *offscrn;
extern SDL_Surface *groundgfx;
extern SDL_Surface *spritegfx;
extern SDL_Surface *fontgfx;


#endif
