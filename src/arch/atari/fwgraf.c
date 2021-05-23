/*************************************************************/
/* fwgraf.c - graphical output routines of Fanwor            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth, 1999                      */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#pragma -CUS

#include <aes.h>
#include <vdi.h>

#include <stdlib.h>
#include <macros.h>     /* For min() und max() */
#include <string.h>

#include "loadimg.h"
#include "fwmain.h"
#include "fanwor.h"
#include "fwguiini.h"
#include "fwgui.h"


/* **Variables:** */
int bipp;                               /* Bits per pixel */
MFDB scrnmfdb= {0L, 0, 0, 0, 0, 0, 0, 0, 0}; /* The MFDB of the screen */
MFDB offscr;                            /* Offscreen MFDB */
MFDB groundfdb;                         /* For the ground graphics */
MFDB fontfdb;                           /* For the font graphics */
MFDB spritefdb;
MFDB spritemask;
unsigned int new_pal[256][3];           /* Die Palette */



/* ***Palette setzen*** */
void setpal(unsigned short palette[][3])
{
	register int i;
	int work_out[57];

	vq_extnd(vhndl, 0, work_out);

	for(i=0; i<work_out[13]; i++)
	{
		vs_color(vhndl, i, &palette[i][0]);
	}
}


/* ***Draw window using the offscreen bitmap*** */
void drwindow(GRECT *xywh)
{
	GRECT clip;
	int xy[10];

	wind_update(BEG_UPDATE);
	graf_mouse(M_OFF, 0L);

	wind_get(wihndl, WF_FIRSTXYWH, &clip.g_x, &clip.g_y, &clip.g_w, &clip.g_h);
	while(clip.g_w!=0 && clip.g_h!=0)
	{
		if( rc_intersect(xywh, &clip) )
		{
			clip.g_w+=clip.g_x-1;
			clip.g_h+=clip.g_y-1;
			xy[0]=clip.g_x-wi.g_x;
			xy[1]=clip.g_y-wi.g_y;
			xy[2]=clip.g_w-wi.g_x;
			xy[3]=clip.g_h-wi.g_y;
			xy[4]=clip.g_x;
			xy[5]=clip.g_y;
			xy[6]=clip.g_w;
			xy[7]=clip.g_h;
			vro_cpyfm(vhndl, 3, xy, &offscr, &scrnmfdb);
		}
		wind_get(wihndl, WF_NEXTXYWH, &clip.g_x, &clip.g_y, &clip.g_w, &clip.g_h);
	}

	graf_mouse(M_ON, 0L);
	wind_update(END_UPDATE);
}

/* ***Map a offscreen section to the window*** */
void offscr2win(short xpos, short ypos, short width, short height)
{
	GRECT xywh;

	xywh.g_x=(xpos-rwx)*32+wi.g_x;
	xywh.g_y=(ypos-rwy)*32+wi.g_y;
	xywh.g_w=width*32;
	xywh.g_h=height*32;

	drwindow(&xywh);
}


/* *** Draw a ground block into the offscreen *** */
void drawblock(short xpos, short ypos, short num)
{
	int xy[8];
	if(xpos<rwx || xpos>=rwx+rww || ypos<rwy || ypos>=rwy+rwh) return;
	xy[0]=0;
	xy[1]=num*32;
	xy[2]=31;
	xy[3]=xy[1]+31;
	xy[4]=(xpos-rwx)*32;
	xy[5]=(ypos-rwy)*32;
	xy[6]=xy[4]+31;
	xy[7]=xy[5]+31;
	vro_cpyfm(vhndl, 3, xy, &groundfdb, &offscr);
}



/* ***Hitpoints (und Goldstand) ausgeben*** */
void write_hp_gold()
{
	int xy[8];
	GRECT rdrect;

	xy[0]=0;
	xy[1]=16*('0'+sprites[0].hp%10);
	xy[2]=15;
	xy[3]=xy[1]+15;
	xy[4]=112;
	xy[5]=360;
	xy[6]=xy[4]+15;
	xy[7]=xy[5]+15;
	vro_cpyfm(vhndl, 3, xy, &fontfdb, &offscr);
	xy[1]=16*('0'+sprites[0].hp/10);
	xy[3]=xy[1]+15;
	xy[4]-=16;
	xy[6]=xy[4]+15;
	vro_cpyfm(vhndl, 3, xy, &fontfdb, &offscr);
	rdrect.g_x=wi.g_x+96;
	rdrect.g_y=wi.g_y+360;
	rdrect.g_w=32;
	rdrect.g_h=16;
	drwindow(&rdrect);
}



/* ***Clear a sprite from the screen*** */
void clearsprite(short nr)
{
	int dxy[8];
	GRECT oclip, clip, sprct;
	int rwxkoor, rwykoor;
	register SPRT *aktsprt;
	int mausflag=FALSE;
	short maus_x, maus_y;
	char rwflag;

	rwxkoor=rwx<<5;
	rwykoor=rwy<<5;
	rwflag=0;
	aktsprt=&sprites[nr];

	/** Redraw background: **/
	dxy[0]=0;
	dxy[2]=31;
	if(aktsprt->oldxpos>=rwx && aktsprt->oldypos>=rwy)
	{
		dxy[1]=room[aktsprt->oldxpos][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
		dxy[3]=((dxy[1]+32)&0xFFE0)-1;
		dxy[4]=(aktsprt->oldx-rwxkoor)&0xFFE0;
		dxy[5]=aktsprt->oldy-rwykoor;
		dxy[6]=dxy[4]+31;
		dxy[7]=((dxy[5]+32)&0xFFE0)-1;
		vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
		rwflag=1;
	}
	if(aktsprt->oldy&0x01F && aktsprt->oldxpos>=rwx && aktsprt->oldypos+1<rwy+(short)rwh)
	{
		dxy[1]=room[aktsprt->oldxpos][aktsprt->oldypos+1]*32;
		dxy[3]=dxy[1]+(aktsprt->oldy&0x01F)-1;
		dxy[4]=(aktsprt->oldx-rwxkoor)&0xFFE0;
		dxy[5]=(aktsprt->oldy+32-rwykoor)&0xFFE0;
		dxy[6]=dxy[4]+31;
		dxy[7]=aktsprt->oldy+31-rwykoor;
		vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
		rwflag=1;
	}
	if(aktsprt->oldx&0x01F && aktsprt->oldxpos+1<rwx+(short)rww && aktsprt->oldypos>=rwy)
	{
		dxy[1]=room[aktsprt->oldxpos+1][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
		dxy[3]=((dxy[1]+32)&0xFFE0)-1;
		dxy[4]=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
		dxy[5]=aktsprt->oldy-rwykoor;
		dxy[6]=dxy[4]+31;
		dxy[7]=((dxy[5]+32)&0xFFE0)-1;
		vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
		rwflag=1;
	}
	if((aktsprt->oldx&0x01F) && (aktsprt->oldy&0x01F) && aktsprt->oldxpos+1<rwx+(short)rww && aktsprt->oldypos+1<rwy+(short)rwh)
	{
		dxy[1]=room[aktsprt->oldxpos+1][aktsprt->oldypos+1]*32;
		dxy[3]=dxy[1]+(aktsprt->oldy&0x01F)-1;
		dxy[4]=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
		dxy[5]=(aktsprt->oldy+32-rwykoor)&0xFFE0;
		dxy[6]=dxy[4]+31;
		dxy[7]=aktsprt->oldy+31-rwykoor;
		rwflag=1;
		vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
	}

	/** Jetzt ins Fenster kopieren: **/
	if(rwflag)
	{
		graf_mkstate(&maus_x, &maus_y, dxy, dxy); /* Aktuelle Mauspos. holen */
		wind_update(BEG_UPDATE);
		wind_get(wihndl, WF_FIRSTXYWH, &oclip.g_x, &oclip.g_y, &oclip.g_w, &oclip.g_h);
		while(oclip.g_w!=0 && oclip.g_h!=0)
		{
			clip.g_x=oclip.g_x;
			clip.g_y=oclip.g_y;
			clip.g_w=oclip.g_w;
			clip.g_h=oclip.g_h;
			sprct.g_x=aktsprt->oldx-rwxkoor+wi.g_x;
			sprct.g_y=aktsprt->oldy-rwykoor+wi.g_y;
			sprct.g_w=aktsprt->oldx+32-rwxkoor+wi.g_x-sprct.g_x;
			sprct.g_h=aktsprt->oldy+32-rwykoor+wi.g_y-sprct.g_y;
			if( rc_intersect(&sprct, &clip) )
			{
				dxy[0]=clip.g_x-wi.g_x;
				dxy[1]=clip.g_y-wi.g_y;
				dxy[2]=dxy[0]+clip.g_w-1;
				dxy[3]=dxy[1]+clip.g_h-1;
				dxy[4]=clip.g_x;
				dxy[5]=clip.g_y;
				dxy[6]=dxy[4]+clip.g_w-1;
				dxy[7]=dxy[5]+clip.g_h-1;
				if(maus_x+16>dxy[4] && maus_x-8<dxy[6] && maus_y+16>dxy[5] && maus_y-8<dxy[7])
				{
					mausflag=TRUE;        /* Maus loeschen */
					graf_mouse(M_OFF, 0L);
				}
				vro_cpyfm(vhndl, 3, dxy, &offscr, &scrnmfdb); /* Ins Fenster kopieren */
				if(mausflag)
				{
					mausflag=FALSE;        /* Maus wieder an */
					graf_mouse(M_ON, 0L);
				}
			}
			wind_get(wihndl, WF_NEXTXYWH, &oclip.g_x, &oclip.g_y, &oclip.g_w, &oclip.g_h);
		}
		wind_update(END_UPDATE);
	}
}



/* ***Draw the sprites*** */
void drawsprites(void)
{
	register int i;
	int dxy[8];
	GRECT oclip, clip, sprct;
	int rwxkoor, rwykoor;
	register SPRT *aktsprt;
	int mausflag=FALSE;
	short maus_x, maus_y;
	char rdsprflags[257]; /* 0=Sprite muss nicht neu gezeichnet werden, 1=Sprite neu zeichnen */

	rwxkoor=rwx<<5;
	rwykoor=rwy<<5;

	/** Redraw background: **/
	dxy[0]=0;
	dxy[2]=31;
	for(i=0; i<spritenr; i++)
	{
		aktsprt=&sprites[i];
		if(aktsprt->hp>=0 && aktsprt->xpos>=(unsigned char)rwx && aktsprt->ypos>=(unsigned char)rwy
		                && aktsprt->xpos<rwx+rww && aktsprt->ypos<rwy+rwh )
		{
			if(aktsprt->oldxpos>=rwx && aktsprt->oldypos>=rwy)
			{
				dxy[1]=room[aktsprt->oldxpos][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
				dxy[3]=((dxy[1]+32)&0xFFE0)-1;
				dxy[4]=(aktsprt->oldx-rwxkoor)&0xFFE0;
				dxy[5]=aktsprt->oldy-rwykoor;
				dxy[6]=dxy[4]+31;
				dxy[7]=((dxy[5]+32)&0xFFE0)-1;
				vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
			}
			if(aktsprt->oldy&0x01F && aktsprt->oldxpos>=rwx && aktsprt->oldypos+1<rwy+(short)rwh)
			{
				dxy[1]=room[aktsprt->oldxpos][aktsprt->oldypos+1]*32;
				dxy[3]=dxy[1]+(aktsprt->oldy&0x01F)-1;
				dxy[4]=(aktsprt->oldx-rwxkoor)&0xFFE0;
				dxy[5]=(aktsprt->oldy+32-rwykoor)&0xFFE0;
				dxy[6]=dxy[4]+31;
				dxy[7]=aktsprt->oldy+31-rwykoor;
				vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
			}
			if(aktsprt->oldx&0x01F && aktsprt->oldxpos+1<rwx+(short)rww && aktsprt->oldypos>=rwy)
			{
				dxy[1]=room[aktsprt->oldxpos+1][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
				dxy[3]=((dxy[1]+32)&0xFFE0)-1;
				dxy[4]=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
				dxy[5]=aktsprt->oldy-rwykoor;
				dxy[6]=dxy[4]+31;
				dxy[7]=((dxy[5]+32)&0xFFE0)-1;
				vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
			}
			if((aktsprt->oldx&0x01F) && (aktsprt->oldy&0x01F) && aktsprt->oldxpos+1<rwx+(short)rww && aktsprt->oldypos+1<rwy+(short)rwh)
			{
				dxy[1]=room[aktsprt->oldxpos+1][aktsprt->oldypos+1]*32;
				dxy[3]=dxy[1]+(aktsprt->oldy&0x01F)-1;
				dxy[4]=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
				dxy[5]=(aktsprt->oldy+32-rwykoor)&0xFFE0;
				dxy[6]=dxy[4]+31;
				dxy[7]=aktsprt->oldy+31-rwykoor;
				vro_cpyfm(vhndl, 3, dxy, &groundfdb, &offscr);
			}
			rdsprflags[i+1]=1;
		}
		else rdsprflags[i+1]=0;   /* Out of the window => do not redraw */
	}

	/** Now draw the sprites: **/
	for(i=0; i<spritenr; i++)
	{
		aktsprt=&sprites[i];
		if(rdsprflags[i+1])
		{
			dxy[1]=aktsprt->grfyoffset[aktsprt->directn-1]+(aktsprt->anim*32);
			dxy[3]=dxy[1]+31;
			dxy[4]=aktsprt->x-rwxkoor;
			dxy[5]=aktsprt->y-rwykoor;
			dxy[6]=dxy[4]+31;
			dxy[7]=dxy[5]+31;
			if(dxy[4]<0 || dxy[5]<0 || dxy[6]>=(int)(rww<<5) || dxy[7]>=(int)(rwh<<5))
				continue;
			vro_cpyfm(vhndl, 1, dxy, &spritemask, &offscr);
			vro_cpyfm(vhndl, 7, dxy, &spritefdb, &offscr);   /* Sprite zeichnen */
		}
	}

	/** Jetzt ins Fenster kopieren: **/
	graf_mkstate(&maus_x, &maus_y, dxy, dxy); /* Aktuelle Mauspos. holen */
	wind_update(BEG_UPDATE);
	wind_get(wihndl, WF_FIRSTXYWH, &oclip.g_x, &oclip.g_y, &oclip.g_w, &oclip.g_h);
	while(oclip.g_w!=0 && oclip.g_h!=0)
	{
		for(i=0; i<spritenr; i++)
		{
			aktsprt=&sprites[i];
			if(!rdsprflags[i+1])
				continue; /* Nur wenn noetig: Spart einiges an CPU-Zeit */
			clip.g_x=oclip.g_x;
			clip.g_y=oclip.g_y;
			clip.g_w=oclip.g_w;
			clip.g_h=oclip.g_h;
			sprct.g_x=min(aktsprt->x, aktsprt->oldx)-rwxkoor+wi.g_x;
			sprct.g_y=min(aktsprt->y, aktsprt->oldy)-rwykoor+wi.g_y;
			sprct.g_w=max(aktsprt->x, aktsprt->oldx)+32-rwxkoor+wi.g_x-sprct.g_x;
			sprct.g_h=max(aktsprt->y, aktsprt->oldy)+32-rwykoor+wi.g_y-sprct.g_y;
			if( rc_intersect(&sprct, &clip) )
			{
				dxy[0]=clip.g_x-wi.g_x;
				dxy[1]=clip.g_y-wi.g_y;
				dxy[2]=dxy[0]+clip.g_w-1;
				dxy[3]=dxy[1]+clip.g_h-1;
				dxy[4]=clip.g_x;
				dxy[5]=clip.g_y;
				dxy[6]=dxy[4]+clip.g_w-1;
				dxy[7]=dxy[5]+clip.g_h-1;
				if(maus_x+16>dxy[4] && maus_x-8<dxy[6] && maus_y+16>dxy[5] && maus_y-8<dxy[7])
				{
					mausflag=TRUE;        /* Maus loeschen */
					graf_mouse(M_OFF, 0L);
				}
				vro_cpyfm(vhndl, 3, dxy, &offscr, &scrnmfdb); /* Ins Fenster kopieren */
				/*v_pline(vhndl, 2, &dxy[4]);*/ /*Zum Debuggen: Bereich markieren */
				if(mausflag)
				{
					mausflag=FALSE;        /* Maus wieder an */
					graf_mouse(M_ON, 0L);
				}
			}
		}  /* for */
		wind_get(wihndl, WF_NEXTXYWH, &oclip.g_x, &oclip.g_y, &oclip.g_w, &oclip.g_h);
	}
	wind_update(END_UPDATE);

}


/* ***Show a picture and wait for a key*** */
int showpicture(char *name)
{
	char fullname[128];
	int wichevnt, i;
	int ppal[256][3];
	int xy[8];
	GRECT rdrct;

	strcpy(fullname, "graphics\\");
	strcat(fullname, name);
	strcat(fullname, "0.img");

	if(bipp<=8)
		fullname[strlen(fullname)-5]+=bipp;
	else
		fullname[strlen(fullname)-5]='8';

	if( getximgpal(fullname, ppal)==0 )
		setpal(ppal);

	if(bipp<=8)
		i=loadpic(fullname, &offscr, 0);
	else
		i=loadpic2true(fullname, &offscr, 0);
	if(i)
	{
		form_alert(1,"[3][Could not load|the picture][Ok]");
		return(-1);
	}

	drwindow(&wi);

	clearkeybuf();
	do
	{
		wichevnt=evnt_multi(MU_TIMER|MU_KEYBD, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,
		                    msgbuf, 800, 0, &mausx, &mausy, &mausk, &kstate, &key, &klicks);
	}
	while(!(wichevnt&MU_TIMER));

	do
	{
		wichevnt=evnt_multi(MU_MESAG|MU_KEYBD, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,
		                    msgbuf, 0, 0, &mausx, &mausy, &mausk, &kstate, &key, &klicks);

		if(wichevnt & MU_MESAG)
			mesages();
	}
	while(!(wichevnt&MU_KEYBD) && !flag_quit);

	clearkeybuf();
	do
	{
		wichevnt=evnt_multi(MU_TIMER|MU_KEYBD, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,
		                    msgbuf, 400, 0, &mausx, &mausy, &mausk, &kstate, &key, &klicks);
	}
	while(!(wichevnt&MU_TIMER));

	setpal(new_pal);

	xy[0]=xy[4]=0;
	xy[1]=xy[5]=rwh*32;
	xy[2]=xy[6]=rww*32-1;
	xy[7]=xy[3]=xy[1]+31;
	vro_cpyfm(vhndl, (bipp<=8)?ALL_BLACK:ALL_WHITE, xy, &offscr, &offscr);
	xy[0]=0;
	xy[1]=0;
	xy[2]=15;
	xy[3]=15;
	xy[4]=64;
	xy[5]=360;
	xy[6]=xy[4]+15;
	xy[7]=xy[5]+15;
	vro_cpyfm(vhndl, 3, xy, &fontfdb, &offscr);
	xy[1]='='*16;
	xy[3]=xy[1]+15;
	xy[4]+=16;
	xy[6]=xy[4]+15;
	vro_cpyfm(vhndl, 3, xy, &fontfdb, &offscr);
	rdrct.g_x=wi.g_x;
	rdrct.g_w=rww*32;
	rdrct.g_y=wi.g_y+rwh*32;
	rdrct.g_h=32;
	drwindow(&rdrct);
}
