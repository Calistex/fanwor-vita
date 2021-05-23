
#ifndef _FWMAIN_H
#define _FWMAIN_H

#include "fwdefs.h"

void addsprite(short index, short x, short y);
void delsprite(short nr);

extern int g_argc;
extern char **g_argv;
extern short rwx, rwy, rww, rwh;
extern short r_width, r_height;
extern unsigned char room[128][128];
extern unsigned char ffield[128][128];
extern char room_x, room_y;
extern unsigned char roomnr;
extern SPRT sprites[];
extern short spritenr;
extern unsigned char tdirectn;
extern short gold;
extern DOOR doors[MAXDOORNR];
extern short doornr;
extern short alrdyhit;
extern char flag_ende;
extern char flag_quit;

#endif
