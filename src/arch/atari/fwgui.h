
#ifndef _FWGUI_H
#define _FWGUI_H

#include <aes.h>
#include <vdi.h>

void event_handler();
int choicedlg(char *txt, char *buts, int defb);
void alertdlg(char *alstr);
void errfatldlg(char *errstr);
void waitms(short ms);
void mesages(void);

#define WINDOWGADGETS (CLOSER|MOVER|NAME)

extern int ap_id;
extern int vhndl;
extern int deskx, desky, deskw, deskh;
extern int wihndl;
extern GRECT wi;
extern OBJECT *menudlg;
extern int mausx, mausy, mausk, klicks;
extern int kstate, key;
extern short mb_pressed;
extern int msgbuf[8];
extern int deskclip[4];

#endif
