
#ifndef _FWGUI_H
#define _FWGUI_H

void event_handler(void);
int choicedlg(char *txt, char *buts, int defb);
void alertdlg(char *alstr);
void errfatldlg(char *errstr);
void waitms(short ms);

enum
{
	GJ_TRIANGLE = 0,
	GJ_CIRCLE = 1,
	GJ_CROSS = 2,
	GJ_SQUARE = 3,
	GJ_L = 4,
	GJ_R = 5,
	GJ_DOWN = 6,
	GJ_LEFT = 7,
	GJ_UP = 8,
	GJ_RIGHT = 9,
	GJ_SELECT = 10,
	GJ_START = 11
};

#endif
