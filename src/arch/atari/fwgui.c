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

#pragma -CUS

#include <osbind.h>
#include <aes.h>
#include <vdi.h>

#include <stdlib.h>
#include <string.h>

#include "windial.h"
#include "fwguiini.h"
#include "fwmain.h"
#include "fwgui.h"
#include "fwgraf.h"
#include "fanwor.h"
#include "fwmusic.h"



/* *Variables:* */
int ap_id;                              /* AES-Handle */
int vhndl;                              /* VDI-Handle */
int deskx, desky, deskw, deskh;         /* Deskto size */
int wihndl;                             /* Fenster-Handle */
GRECT wi;                               /* Fenster-Koordinaten */
OBJECT *menudlg;
int mausx, mausy, mausk, klicks;        /* Zur Mausabfrage */
int kstate, key;                        /* Zur Tastaturabfrage */
short mb_pressed;                       /* Has the mouse button been pressed? */
int msgbuf[8];                          /* Der Nachrichtenbuffer */
short wichevnt;                         /* Aufgetretene Evnt-Multi-Ereignisse */
int deskclip[4];                        /* To clip to the desktop rectangle */
int gamespeed=0;

short flag_key=1;
short flag_joy=0;
short flag_pad=0;
unsigned char joystate;                  /* Joystickstatus */


/* ***Tastenbuffer loeschen*** */
void clearkeybuf()
{
	static IOREC *keybufp=0L;
	if(!keybufp) keybufp=Iorec(1);
	keybufp->ibuftl=keybufp->ibufhd;
}



/* ***Wait some milliseconds*** */
void waitms(short ms)
{
	evnt_timer(ms, 0);
}


/* ***Show an choice box*** */
int choicedlg(char *txt, char *buts, int defb)
{
	char f_a_str[256];
	int i;
	strcpy(f_a_str, "[2][");
	strcat(f_a_str, txt);
	strcat(f_a_str, "][");
	strcat(f_a_str, buts);
	strcat(f_a_str, "]");
	for(i=0; i<strlen(f_a_str); i++)
		if(f_a_str[i]=='\n') f_a_str[i]='|';
	return( form_alert(defb, f_a_str) );
}

/* ***Show an alert box*** */
void alertdlg(char *alstr)
{
	char f_a_str[256];
	int i;
	for(i=0; i<strlen(alstr); i++)
		if(alstr[i]=='\n') alstr[i]='|';
	strcpy(f_a_str, "[3][");
	strcat(f_a_str, alstr);
	strcat(f_a_str, "][Ok]");
	form_alert(1, f_a_str);
}

/* ***Show a fatal error message, then quit*** */
void errfatldlg(char *errstr)
{
	char newerrstr[256];
	int i;
	for(i=0; i<strlen(errstr); i++)
		if(errstr[i]=='\n') errstr[i]='|';
	strcpy(newerrstr, "[3][");
	strcat(newerrstr, errstr);
	strcat(newerrstr, "][Cancel]");
	form_alert(1, newerrstr);
	close_window();
	exitGUI();
	exit(-1);
}


/* *** Message handler for the window dialogs *** */
void wdmsgs(int msgbf[])
{
	if( msgbf[0]==WM_REDRAW )  drwindow((GRECT *)&msgbf[4]);
	if( msgbf[0]==WM_MOVED )
	{
		wind_set(msgbf[3], WF_CURRXYWH, msgbf[4], msgbf[5], msgbf[6], msgbf[7]);
		wind_calc(WC_WORK, WINDOWGADGETS, msgbf[4], msgbf[5], msgbf[6], msgbf[7],
		          &wi.g_x, &wi.g_y, &wi.g_w, &wi.g_h);
	}
}


/* ***User has selected a menu entry*** */
void reactmenu()
{
	int wdh, exitbut, menuindex;
	OBJECT *dlgptr;

	menuindex=msgbuf[3];

	switch(msgbuf[4])
	{
	case ABOUTIT:                       /* Show Copyright */
		rsrc_gaddr(R_TREE, ABOUTDLG, &dlgptr);
		wdh=wdial_init(dlgptr, "About Fanwor");
		wdial_formdo(wdh, dlgptr, 0, wdmsgs, gamespeed, NULL);
		wdial_close(wdh);
		dlgptr[ABOUTOK].ob_state=NORMAL;
		break;
	case NEWGAME:
		if( form_alert(2,"[2][Restart game?][Yes|No]")==1 )
			sprites[0].hp=0;
		break;
	case QUITGAME:                        /* Spiel verlassen */
		if( form_alert(2,"[2][Quit the game?][Yes|No]")==1 )
			flag_quit=TRUE;
		break;
	case MENCNTRL:
		rsrc_gaddr(R_TREE, CNTRLDLG, &dlgptr);
		wdh=wdial_init(dlgptr, "Fanwor controls");
		wdial_formdo(wdh, dlgptr, 0, wdmsgs, gamespeed, NULL);
		wdial_close(wdh);
		dlgptr[CNTRLOK].ob_state=NORMAL;
		flag_joy=dlgptr[CUSEJOY].ob_state&SELECTED;
		flag_pad=dlgptr[CUSEPAD].ob_state&SELECTED;
		if( flag_key && !(dlgptr[CUSEKEY].ob_state&SELECTED) )
		{
			long save_ssp;
			Kbrate(oldkbrate>>8, oldkbrate & 0x0FF);
			save_ssp=(long)Super(0L);
			*(unsigned char *)(0x484)=oldconterm;		/* Tastaturklick ein */
			Super(save_ssp);
			flag_key=0;
		}
		if( !flag_key && (dlgptr[CUSEKEY].ob_state&SELECTED) )
		{
			long save_ssp;
			Kbrate(1, 1);					/* Tastaturwiederholung klein setzen */
			save_ssp=(long)Super(0L);
			*(unsigned char *)(0x484)=0x0A;					/* Tastaturklick aus */
			Super(save_ssp);
		}
		break;
	case MENSPEED:
		rsrc_gaddr(R_TREE, SPEEDDLG, &dlgptr);
		wdh=wdial_init(dlgptr, "Game Speed");
		do
		{
			exitbut=wdial_formdo(wdh, dlgptr, 0, wdmsgs, gamespeed, NULL) & 0x7FFF;
			if(exitbut==SPDLARRW) /* switch() scheint hier nicht richtig zu funken? */
			{
				if(gamespeed>0)  --gamespeed;
				else exitbut=0;
			}
			if(exitbut==SPDRARRW)
			{
				if(gamespeed<50)  ++gamespeed;
				else exitbut=0;
			}
			if(exitbut==SPDSLIDR)
				gamespeed=50L*graf_slidebox(dlgptr, SPDSLPAR, SPDSLIDR, 0)/1000;
			if(exitbut!=SPEEDOK)
			{
				dlgptr[SPDSLIDR].ob_x=gamespeed*(dlgptr[SPDSLPAR].ob_width-dlgptr[SPDSLIDR].ob_width)/50;
				dlgptr[SPDSLIDR].ob_spec.tedinfo->te_ptext[0]='0'+gamespeed/10;
				dlgptr[SPDSLIDR].ob_spec.tedinfo->te_ptext[1]='0'+gamespeed%10;
				objc_draw(dlgptr, SPDSLPAR, 2, dlgptr->ob_x, dlgptr->ob_y,
				          dlgptr->ob_width, dlgptr->ob_height);
			}
		}
		while(exitbut!=SPEEDOK);
		wdial_close(wdh);
		dlgptr[SPEEDOK].ob_state=NORMAL;
		if(dlgptr[CLIPFLAG].ob_state&SELECTED)
			vs_clip(vhndl, 1, deskclip);
		else
			vs_clip(vhndl, 0, deskclip);
		break;
	case MENSOUND:                      /* Sounddialog */
		rsrc_gaddr(R_TREE, SOUNDDLG, &dlgptr);
		if(!ptyp)  dlgptr[MODBGSND].ob_state|=DISABLED;
		wdh=wdial_init(dlgptr, "Sound options");
		do
		{
			exitbut=wdial_formdo(wdh, dlgptr, 0, wdmsgs, gamespeed, NULL) & 0x7FFF;
		}
		while(exitbut!=SOUNDOK);
		wdial_close(wdh);
		dlgptr[SOUNDOK].ob_state=NORMAL;
		sndmodflag=dlgptr[MODBGSND].ob_state&SELECTED;
		sndpsgflag=dlgptr[PSGSND].ob_state&SELECTED;
		sndsamflag=dlgptr[SAMSND].ob_state&SELECTED;
		if(sndmodflag && ptyp && !playflag)
			mod_play("sounds\\backgrnd.mod");
		if(!sndmodflag && ptyp && playflag)
			mod_stop();
		if(sndmodflag && ptyp!=1)
		{
			sndpsgflag=1;
			sndsamflag=0;
			dlgptr[PSGSND].ob_state|=SELECTED;
			dlgptr[SAMSND].ob_state&=~SELECTED;
		}
		break;
	}

	menu_tnormal(menudlg, menuindex, 1);
}



/* ***Tastendruecke*** */
void keyklicks(void)
{
	int i,d;
	char scancode;

	scancode=key>>8;

	if(kstate<=3)
	{
		switch(scancode)
		{
		case 0x48:
			tdirectn=1;
			break;
		case 0x4D:
			tdirectn=2;
			break;
		case 0x50:
			tdirectn=3;
			break;
		case 0x4B:
			tdirectn=4;
			break;
		case 0x39:
			tdirectn=128;
			break;
		case 0x19:
			while((char)evnt_keybd()=='p');
			break; /* Pause bis Tastendruck! */
		case 0x10:
			if( form_alert(2,"[2][Quit the game?][Yes|No]")==1 )
				flag_quit=TRUE;
			break;
		}
		clearkeybuf();
	}

}



/* ***Nachrichten*** */
void mesages(void)
{
	int xy[8];
	short omx, omy;

	switch(msgbuf[0])
	{
	case MN_SELECTED:
		reactmenu();
		break;
	case WM_REDRAW:
		drwindow((GRECT *)&msgbuf[4]);
		break;
	case WM_TOPPED:
		wind_set(msgbuf[3], WF_TOP, 0, 0, 0, 0);
		setpal(new_pal);
		break;
	case WM_MOVED:
		wind_set(msgbuf[3], WF_CURRXYWH, msgbuf[4], msgbuf[5], msgbuf[6], msgbuf[7]);
		wind_calc(WC_WORK, WINDOWGADGETS, msgbuf[4], msgbuf[5],
		          msgbuf[6], msgbuf[7], &wi.g_x, &wi.g_y, &wi.g_w, &wi.g_h);
		break;
	case WM_CLOSED:
		if( form_alert(2,"[2][Quit the game?][Yes|No]")==2 )
			break;
	case AP_TERM:                                     /* Shutdown? */
	case AP_RESCHG:
		flag_quit=TRUE;
		break;
	}
}



/* ***Get the joypad state*** */
void getjoypad()
{
	*(unsigned short *)0xFFFF9202=0xFFEE;
	joystate=~(*(unsigned char *)0xFFFF9202);
	*(unsigned short *)0xFFFF9202=0xEEEE;
	if((~(*(unsigned short *)0xFFFF9200)) & 2) joystate|=128;
}


/* ***Event function*** */
void event_handler()
{
	wichevnt=evnt_multi(MU_TIMER|MU_MESAG|MU_KEYBD|MU_BUTTON,
	                    /*(alrdyhit?0:1)*/1, 2, 2,
	                    0,0,0,0,0,0,0,0,0,0,
	                    msgbuf, 0, 0, &mausx, &mausy, &mausk, &kstate, &key, &klicks);

	if(wichevnt & MU_MESAG)
		mesages();

	tdirectn=0;
	if(wichevnt & MU_KEYBD)
		keyklicks();
	if(flag_pad)			/* Jaguarpad abfragen */
		Supexec(getjoypad);
	if( (flag_joy || flag_pad) && joystate )
	{
		switch(joystate)
		{
		case 1:
			tdirectn=1;
			break;
		case 8:
			tdirectn=2;
			break;
		case 2:
			tdirectn=3;
			break;
		case 4:
			tdirectn=4;
			break;
		default:
			tdirectn=0;
			break;
		}
		if(joystate>=128)  tdirectn|=128;
	}
	if( kstate || ((wichevnt&MU_BUTTON) && mausk) )
		tdirectn|=128;

	if(gamespeed)  evnt_timer(gamespeed, 0);
}
