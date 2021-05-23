
#ifndef _FWGUIINI_H
#define _FWGUIINI_H

#include <vdi.h>

extern unsigned short oldkbrate;
extern unsigned char oldconterm;

int initGUI(void);
void exitGUI(void);
int open_window(void);
void close_window(void);
long initgraf(void);
int loadpic(char *pname, MFDB *destfdb, short newfdb);
int loadpic2true(char *pname, MFDB *destfdb, short newmem);
int xgetcookie(long cookie, long *value);

#endif
