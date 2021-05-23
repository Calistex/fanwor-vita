/* *** fwgraf.h - Variables and prototypes of fwgraf.c *** */
/* *** Written and (c) by Thomas Huth, 1999 *** */

#ifndef _FWGRAF_H
#define _FWGRAF_H

#include <aes.h>
#include <vdi.h>

void drawsprites();
void clearsprite(short nr);
void drawblock(short xpos, short ypos, short num);
void offscr2win(short xpos, short ypos, short width, short height);
void drwindow(GRECT *xywh);
void write_hp_gold();
void setpal(unsigned short palette[][3]);
int showpicture(char *name);

extern int bipp;
extern MFDB scrnmfdb;
extern MFDB offscr;
extern MFDB fontfdb;
extern MFDB groundfdb;
extern MFDB spritefdb;
extern MFDB spritemask;
extern unsigned int new_pal[256][3];

#endif
