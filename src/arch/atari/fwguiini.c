/*************************************************************/
/* fwguiini.c  -  Graphical User Interface init routines     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <string.h>

#define ATARIGEM

#include <aes.h>
#include <vdi.h>

#ifdef ATARIGEM
#include <osbind.h>
#endif


#include "loadimg.h"
#include "fwgui.h"
#include "fwgraf.h"
#include "fwmain.h"
#include "fanwor.h"

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


#ifdef SOZOBON
#define aesversion gl_apversion
#else
/*#define aesversion _GemParBlk.global[0]*/
extern int global[15];
#define aesversion global[0]
#endif

#ifndef Ssystem
#define Ssystem(mode,arg1,arg2)  trap_1_wwll(0x154,(int)(mode),(long)(arg1),(long)(arg2))
#endif


/* ** Prototypes: ** */
void joyinit(void);                                        /* => FW_ASM.S */
void joyrstor(void);

/* Variables: */
unsigned short oldkbrate;
unsigned char oldconterm;
unsigned short old_pal[256][3];       /* Die alte Palette */

char *spritename="graphics\\sprites#.img";
char *groundname="graphics\\ground#.img";
char *fontname="graphics\\font#.img";

long huntc_cookie;
long *huntc_value;


#ifdef ATARIGEM

/* *** Search a cookie in the cookie jar *** */
int huntcookie()
{
	long *actptr;
	long actcookie, actvalue;
	actptr=(long *) (*(long *)0x5a0);
	if( actptr==NULL )  return FALSE;
	do
	{
		actcookie=*actptr++;
		actvalue=*actptr++;
		if(actcookie==huntc_cookie)
		{
			if( huntc_value!=NULL )
				*huntc_value=actvalue;
			return TRUE;
		}
	}
	while(actcookie!=0L);
	return FALSE;
}

/* *** A new getcookie function *** */
int xgetcookie(long cookie, long *value)
{
	int ret;
	long dummy;
	if (value != NULL)
		ret=Ssystem(8, cookie, value);
	else
		ret=Ssystem(8, cookie, &dummy);
	if (ret != -32)
		return( ret==0 );  /* Okay, Ssystem() did not go wrong */
	/* If Ssystem does not exist we use the old way: */
	huntc_cookie=cookie;
	huntc_value=value;
	return Supexec( huntcookie );
}

#else /* ATARIGEM */

int xgetcookie(long cookie, long *value)
{
	return FALSE;
}

#endif /* ATARIGEM */


/* *** Try to use appl_getinfo  *** */
int appl_xgetinfo(int type, int *out1, int *out2, int *out3, int *out4)
{
	static int axgi_init=FALSE, has_agi;

	if(!axgi_init)      /* Already initialized? */
	{
		/* Check if agi is possible */
		axgi_init=TRUE;
		if (appl_find("?AGI\0\0\0\0") >= 0
		    || aesversion>0x0400
		    || xgetcookie(0x4D616758L, 0L)                        /* 'MagX' */
		    || (aesversion==0x400 && xgetcookie(0x4D694E54L, 0L)) /* 'MiNT' */
		   )
			has_agi=TRUE;
		else
			has_agi=FALSE;
	}

	if(has_agi)
		return( appl_getinfo(type, out1, out2, out3, out4) );
	else
		return(0);
}


/* *** Beim GEM anmelden + Keyboard/Joystick initialisieren *** */
int initGUI(void)
{
	int rgb[3];
	int work_in[12], work_out[57];      /* VDI-Felder */
	int i;
	int agi1, agi2, agi3, agi4;
	long save_ssp;

	ap_id=appl_init();
	if(ap_id==-1) return(-1);
	if(aesversion>=0x400)              /* Wenn moeglich einen Namen anmelden */
		menu_register(ap_id, "  Fanwor\0\0\0");
	graf_mouse(ARROW, 0L);             /* Maus als Pfeil */

	if( !rsrc_load("fanwor.rsc") )
	{
		form_alert(1, "[3][Could not|load <fanwor.rsc>][Mhh]");
		appl_exit();
		return(-1);
	}
	rsrc_gaddr(R_TREE, FWMENU, &menudlg);  /* Adresse holen */

	wind_get(0, WF_WORKXYWH, &deskx, &desky, &deskw, &deskh); /* Desktopgroesse */


	/* VDI initialisieren: */
	vhndl=graf_handle(&i, &i, &i, &i);
	for(i=0; i<10; i++) work_in[i]=1;
	work_in[10]=2;
	v_opnvwk(work_in, &vhndl, work_out);  /* VDI-Station oeffnen */

	for(i=0; i<work_out[13]; i++)    /* Alte Farben sichern */
	{
		vq_color(vhndl, i, 1, rgb);
		old_pal[i][0]=rgb[0];
		old_pal[i][1]=rgb[1];
		old_pal[i][2]=rgb[2];
	}

	vq_extnd(vhndl, 1, work_out);
	bipp=work_out[4];                  /* Bits pro Bitplane ermitteln */

	deskclip[0]=deskx;
	deskclip[1]=desky;
	deskclip[2]=deskx+deskw-1;
	deskclip[3]=desky+deskh-1;
	vs_clip(vhndl, 1, deskclip);

	menu_bar(menudlg, 1);        /* Display the menu */

	/* Try to tell the AES that we understand AP_TERM */
	if( (appl_xgetinfo(10, &agi1, &agi2, &agi3, &agi4) && agi1&0x0FF>=9)
	                || aesversion>=0x0400)
		shel_write(9, 1, 0, 0L, 0L);


#ifdef ATARIGEM
	/* Initialize the keyboard and the joystick: */
	joyinit();									/* Joystickroutine installiern */
	oldkbrate=Kbrate(1, 1);					/* Tastaturwiederholung klein setzen */
	save_ssp=(long)Super(0L);
	oldconterm=*(unsigned char *)(0x484);
	*(unsigned char *)(0x484)=0x0A;					/* Tastaturklick aus */
	Super(save_ssp);
#endif

	return 0;
}


/* ***Beim GEM abmelden*** */
void exitGUI(void)
{
	long save_ssp;

#ifdef ATARIGEM
	Kbrate(oldkbrate>>8, oldkbrate & 0x0FF);
	save_ssp=(long)Super(0L);
	*(unsigned char *)(0x484)=oldconterm;				/* Tastaturklick ein */
	Super(save_ssp);
	joyrstor();
#endif

	setpal(old_pal);
	v_clsvwk(vhndl);
	menu_bar(menudlg,0);
	rsrc_free();
	appl_exit();
}



/* ***Fenster oeffnen*** */
int open_window(void)
{
	int x,y,w,h;

	wind_calc(WC_WORK, WINDOWGADGETS, deskx, desky, deskw, deskh, &x, &y, &w, &h);
	if((int)rww>w/32) rww=w/32;
	if((int)rwh+1>h/32) rwh=h/32-1;
	w=w&0xFFE0;
	h=h&0xFFE0;
	wind_calc(WC_BORDER, WINDOWGADGETS, x, y, w, h, &x, &y, &w, &h);
	wihndl=wind_create(WINDOWGADGETS, deskx, desky, w, h);

	wind_calc(WC_BORDER, WINDOWGADGETS, wi.g_x, wi.g_y, rww<<5, (rwh<<5)+32,
	          &x, &y, &wi.g_w, &wi.g_h);
	if(wi.g_x==0 && wi.g_y==0)
	{
		wi.g_x=deskx+(deskw-wi.g_w)/2;
		wi.g_y=desky+(deskh-wi.g_h)/2;
	}
	else
	{
		wi.g_x=x;
		wi.g_y=y;
		if(wi.g_y<desky) wi.g_y=desky;
	}

	wind_set(wihndl, WF_NAME, "Fanwor", 0L);

	wind_open(wihndl, wi.g_x, wi.g_y, wi.g_w, wi.g_h);
	wind_get(wihndl, WF_WORKXYWH, &wi.g_x, &wi.g_y, &wi.g_w, &wi.g_h);

	return(0);
}


/* ***Close the window*** */
void close_window(void)
{
	wind_close(wihndl);
	wind_delete(wihndl);
}



/* ***Normale Grafik laden und ins aktuelle Format wandeln*** */
int loadpic(char *pname, MFDB *destfdb, short newmem)
{
	MFDB loadfdb;
	char alerttxt[64];
	long psize;

	if( LoadImg(pname, &loadfdb) )
	{
		strcpy(alerttxt, "[3][Could not load|\"");
		strcat(alerttxt, pname);
		strcat(alerttxt, "\"][Cancel]");
		form_alert(1, alerttxt);
		return(-1);
	}

	destfdb->fd_w=loadfdb.fd_w;
	destfdb->fd_h=loadfdb.fd_h;
	destfdb->fd_wdwidth=loadfdb.fd_wdwidth;
	destfdb->fd_stand=0;
	destfdb->fd_nplanes=bipp;
	if(newmem)
	{
		psize=(long)destfdb->fd_w*destfdb->fd_h*bipp/8;
		destfdb->fd_addr=(void *)Mxalloc(psize, 2);
		if( ((signed long)destfdb->fd_addr)==-32L )
			destfdb->fd_addr=(void *)Malloc(psize);
		if( ((signed long)destfdb->fd_addr)<=0L )  return(-1);
	}
	vr_trnfm(vhndl, &loadfdb, destfdb);   /* Ins aktuelle Format wandeln */
	Mfree(loadfdb.fd_addr);

	return(0);
}

/* ***Normale Grafik laden und in Truecolor Grafik wandeln*** */
int loadpic2true(char *pname, MFDB *destfdb, short newmem)
{
	MFDB loadfdb;
	char alerttxt[64];

	if( LoadImg(pname, &loadfdb) )
	{
		strcpy(alerttxt, "[3][Could not load|\"");
		strcat(alerttxt, pname);
		strcat(alerttxt, "\"][Cancel]");
		form_alert(1, alerttxt);
		return(-1);
	}

	if( !transform_truecolor(&loadfdb, (long)loadfdb.fd_wdwidth * loadfdb.fd_h, bipp, vhndl) )
	{
		form_alert(1, "[3][Could not transform|graphic to truecolor][Ok]");
		return(-1);
	}

	if(!newmem) Mfree(destfdb->fd_addr);
	destfdb->fd_addr=loadfdb.fd_addr;
	destfdb->fd_w=loadfdb.fd_w;
	destfdb->fd_h=loadfdb.fd_h;
	destfdb->fd_wdwidth=loadfdb.fd_wdwidth;
	destfdb->fd_stand=0;
	destfdb->fd_nplanes=bipp;

	return(0);
}


/* ***Grafik laden und vorbereiten*** */
long initgraf(void)
{
	MFDB loadfdb, mbuffdb;
	unsigned long picsize1, picsize2;
	register long i, j;
	long *mptrs, *mptrd;
	unsigned short bitbuf;
	char alerttxt[64];
	int xy[8];


	/* Offscreen vorbereiten: */
	/* wind_calc(WC_WORK, WINDOWGADGETS, deskx, desky, deskw, deskh,
	         &xy[0], &xy[1], &xy[2], &xy[3]);*/
	xy[2]=rww*32;
	xy[3]=(rwh+1)*32;
	picsize1=(long)((xy[2]+15)/8)*xy[3]*bipp;
	offscr.fd_addr=(void *)Mxalloc(picsize1, 2); /* Get memory */
	if( ((signed long)offscr.fd_addr)==-32L )
		offscr.fd_addr=(void *)Malloc(picsize1);
	if( (signed long)offscr.fd_addr<=0L )
	{
		form_alert(1,"[3][Not enough memory!][ok]");
		return(-1);
	}
	offscr.fd_addr=((char *)offscr.fd_addr);
	offscr.fd_w=xy[2];
	offscr.fd_h=xy[3];
	offscr.fd_wdwidth=(xy[2]+15)>>4;
	offscr.fd_stand=0;
	offscr.fd_nplanes=bipp;

	/* Grafik der Einheiten laden und vorbereiten: */
	if(bipp<=8)
	{
		for(i=0; i<strlen(spritename); i++)
			if(spritename[i]=='#')
			{
				spritename[i]='0'+bipp;
				break;
			}
	}
	else
	{
		for(i=0; i<strlen(spritename); i++)
			if(spritename[i]=='#')
			{
				spritename[i]='8';
				break;
			}
	}
	/* Set palette: */
	if( getximgpal(spritename, new_pal)==0 )
		setpal(new_pal);
	else
	{
		int work_out[57];
		vq_extnd(vhndl, 0, work_out);
		for(i=0; i<work_out[13]; i++)
			vq_color(vhndl, (int)i, 1, &new_pal[i][0]);
		form_alert(1,"[3][Could not set|color palette!][Ok]");
	}

	if( LoadImg(spritename, &loadfdb) )
	{
		strcpy(alerttxt, "[3][Could not load|\"");
		strcat(alerttxt, spritename);
		strcat(alerttxt, "\"][Cancel]");
		form_alert(1, alerttxt);
		return(-1);
	}
	spritefdb.fd_w=spritemask.fd_w=mbuffdb.fd_w=loadfdb.fd_w;
	spritefdb.fd_h=spritemask.fd_h=mbuffdb.fd_h=loadfdb.fd_h;
	spritefdb.fd_wdwidth=spritemask.fd_wdwidth=mbuffdb.fd_wdwidth=loadfdb.fd_wdwidth;
	spritefdb.fd_stand=spritemask.fd_stand=0;
	spritefdb.fd_nplanes=spritemask.fd_nplanes=bipp;
	mbuffdb.fd_nplanes=loadfdb.fd_nplanes;
	picsize1=(long)spritefdb.fd_h*(4L*bipp);
	picsize2=loadfdb.fd_h*(4L*loadfdb.fd_nplanes);
	spritefdb.fd_addr=(void *)Mxalloc(2L*picsize1, 2);
	if( ((signed long)spritefdb.fd_addr)==-32L )
		spritefdb.fd_addr=(void *)Malloc(2L*picsize1);
	if( (signed long)spritefdb.fd_addr<=0L)   return(-1);
	spritemask.fd_addr=(void *)((char *)spritefdb.fd_addr+picsize1);
	if(bipp<=8)
		vr_trnfm(vhndl, &loadfdb, &spritefdb);
	else
	{
		lmemcpy(spritefdb.fd_addr, loadfdb.fd_addr, picsize2);
		spritefdb.fd_nplanes=loadfdb.fd_nplanes;
		spritefdb.fd_stand=1;
		if( !transform_truecolor(&spritefdb, (long)spritefdb.fd_wdwidth * spritefdb.fd_h, bipp, vhndl) )
			form_alert(1, "[3][Could not transform|sprites to truecolor][Ok]");
	}

	/* Masken erstellen: */
	mbuffdb.fd_addr=(void *)Mxalloc(picsize2, 2);
	if( ((signed long)mbuffdb.fd_addr)==-32L )
		mbuffdb.fd_addr=(void *)Malloc(picsize2);
	if( (signed long)mbuffdb.fd_addr<=0L)
	{
		form_alert(1,"[3][Not enough|temporary memory][Ok]");
		return(-1);
	}
	mbuffdb.fd_stand=0;
	xy[0]=xy[1]=xy[4]=xy[5]=0;
	xy[2]=xy[6]=31;
	xy[3]=xy[7]=mbuffdb.fd_h-1;
	vro_cpyfm(vhndl, 0, xy, &mbuffdb, &mbuffdb); /* First clear the buffer */
	mptrs=(long *)loadfdb.fd_addr;
	for(i=0; i<bipp; i++)
	{
		mptrd=(long *)mbuffdb.fd_addr;
		for(j=0; j<mbuffdb.fd_h; j++)
		{
			*mptrd++ |= *mptrs++;
		}
	}
	for(i=1; i<mbuffdb.fd_nplanes; i++)
	{
		lmemcpy((short *)mbuffdb.fd_addr+i*mbuffdb.fd_h*mbuffdb.fd_wdwidth, mbuffdb.fd_addr, mbuffdb.fd_h*2L*mbuffdb.fd_wdwidth);
	}
	xy[0]=xy[1]=xy[4]=xy[5]=0;
	xy[2]=xy[6]=31;
	xy[3]=xy[7]=mbuffdb.fd_h-1;
	if(bipp<=8)  vro_cpyfm(vhndl, 10, xy, &mbuffdb, &mbuffdb);  /* Invertieren */
	mbuffdb.fd_stand=1;
	if(bipp<=8)
		vr_trnfm(vhndl, &mbuffdb, &spritemask);
	else
	{
		lmemcpy(spritemask.fd_addr, mbuffdb.fd_addr, picsize2);
		spritemask.fd_nplanes=mbuffdb.fd_nplanes;
		spritemask.fd_stand=1;
		if( !transform_truecolor(&spritemask, (long)spritemask.fd_wdwidth * spritemask.fd_h, bipp, vhndl) )
			form_alert(1, "[3][Could not transform|sprite masks to truecolor][Ok]");
		vro_cpyfm(vhndl, 6/*S_XOR_D*/, xy, &spritemask, &spritefdb);
	}

	Mfree(loadfdb.fd_addr);         /* Free unused memory */
	Mfree(mbuffdb.fd_addr);

	/* Bodengrafik laden und vorbereiten: */
	if(bipp<=8)
	{
		for(i=0; i<strlen(groundname); i++)
			if( groundname[i]=='#' )
			{
				groundname[i]='0'+bipp;
				break;
			}
		if( loadpic(groundname, &groundfdb,1) )  return(-1);
	}
	else
	{
		for(i=0; i<strlen(groundname); i++)
			if( groundname[i]=='#' )
			{
				groundname[i]='8';
				break;
			}
		if( loadpic2true(groundname, &groundfdb,1) )  return(-1);
	}

	/* Load the font graphics: */
	if(bipp<=8)
	{
		for(i=0; i<strlen(fontname); i++)
			if( fontname[i]=='#' )
			{
				fontname[i]='0'+bipp;
				break;
			}
		if( loadpic(fontname, &fontfdb,1) )  return(-1);
	}
	else
	{
		for(i=0; i<strlen(fontname); i++)
			if( fontname[i]=='#' )
			{
				fontname[i]='8';
				break;
			}
		if( loadpic2true(fontname, &fontfdb,1) )  return(-1);
	}

	return 0;
}

/* *** De-init the graphics *** */
void deinitgraf()
{
	Mfree( offscr.fd_addr );
	Mfree( groundfdb.fd_addr );
	Mfree( spritefdb.fd_addr );
	Mfree( spritemask.fd_addr );
}
