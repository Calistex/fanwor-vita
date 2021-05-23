/*************************************************************/
/* fwdisc.c  -  Disk IO routines                             */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "level.h"
#include "fwgui.h"
#include "fwgraf.h"
#include "fwmain.h"
#include "fwdata.h"
#include "fwdisk.h"
#include "fwreact.h"

#ifdef SOZOBON
#define USEBACKSLASH
#endif


static unsigned short swap_u16(uint16_t in)
{
	uint16_t out;
	out = in << 8;
	out |= in >> 8;
	return out;
}

static unsigned long swap_u32(uint32_t in)
{
	uint32_t out;
	uint16_t o1, o2;
	o1 = (uint16_t)(in>>16);
	o2 = (uint16_t)in;
	o1 = swap_u16(o1);
	o2 = swap_u16(o2);
	out = ((uint32_t)o2) << 16;
	out |= o1;
	return out;
}


/* ***Load a new room*** */
int loadroom(void)
{
	LEVEL_HEADER hd;
	LEVEL_EINTRAG eintrbuf;
	FILE *fhndl;
	long len;
	static long oldlen=0;
	static char *buf=0L;
	char *sfbuf, *ffbuf;
	int dx, dy, i;
	char roomname[256];

	/* Create file name */
	strcpy(roomname, "app0:rooms");
#ifdef USEBACKSLASH
	strcat(roomname, "\\room");
#else
	strcat(roomname, "/room");
#endif
	i=strlen(roomname);
	if(roomnr<10)
		roomname[i]=roomnr+'0';
	else
		roomname[i]=roomnr-10+'a';
	if(room_x<10)
		roomname[i+1]=room_x+'0';
	else
		roomname[i+1]=room_x-10+'a';
	if(room_y<10)
		roomname[i+2]=room_y+'0';
	else
		roomname[i+2]=room_y-10+'a';
	roomname[i+3]=0;

	fhndl=fopen(roomname, "rb");
	if (fhndl == NULL)
	{
		errfatldlg("Could not open\nlevel file!");
		return -1;
	}

	if (fread(&hd, sizeof(LEVEL_HEADER), 1, fhndl) != 1)
	{
		fclose(fhndl);
		errfatldlg("Could not read\nlevel file!");
		return -1;
	}

	if (hd.hmagic != 0x4641574FL)  /*'FAWO'*/
	{
		hd.hmagic = swap_u32(hd.hmagic);
		hd.version = swap_u16(hd.version);
		hd.anz_obj = swap_u16(hd.anz_obj);
		hd.r_wdth = swap_u16(hd.r_wdth);
		hd.r_hght = swap_u16(hd.r_hght);
	}

	if (hd.hmagic != 0x4641574FL)  /*'FAWO'*/
	{
		char str[200];
		fclose(fhndl);
		sprintf(str, "No Fanwor|level file:|%lx", (long)hd.hmagic);
		errfatldlg(str);
		return -1;
	}

	r_width=hd.r_wdth;
	r_height=hd.r_hght;

	len=2L*sizeof(unsigned char)*r_width*r_height;
	if(len>oldlen)
	{
		if(buf) free(buf);
		buf=malloc(len);
		if(buf==NULL)
		{
			fclose(fhndl);
			errfatldlg("No temporary memory\navailable!");
			return -1;
		}
		oldlen=len;
	}

	if( fread(buf, sizeof(char), len, fhndl) != len )
	{
		free(buf);
		fclose(fhndl);
		errfatldlg("Error while reading\nlevel file!");
		return -1;
	}

	sfbuf=buf;
	for(dy=0; dy<r_height; dy++)
		for(dx=0; dx<r_width; dx++)
		{
			room[dx][dy]=*(sfbuf+dx+dy*r_width);
		}
	ffbuf=sfbuf+r_width*r_height;
	for(dy=0; dy<r_height; dy++)
		for(dx=0; dx<r_width; dx++)
		{
			ffield[dx][dy]=*(ffbuf+dx+dy*r_width);
		}

	spritenr=1;
	doornr=0;
	dx=hd.anz_obj;
	while( dx>0 )
	{
		if( fread(&eintrbuf, sizeof(LEVEL_EINTRAG), 1, fhndl) != 1 )
		{
			fclose(fhndl);
			errfatldlg("Could not load\nlevel file");
			return -1;
		}
		switch(eintrbuf.eintrtyp)
		{
		case 1:
			addsprite(eintrbuf.art, eintrbuf.xpos<<5, eintrbuf.ypos<<5);
			break;
		case 2:
			doors[doornr].x=eintrbuf.xpos;
			doors[doornr].y=eintrbuf.ypos;
			doors[doornr].destnr=eintrbuf.art;
			doors[doornr].exitx=eintrbuf.xl;
			doors[doornr].exity=eintrbuf.yl;
			doors[doornr].destx=eintrbuf.spec1>>4;
			doors[doornr].desty=eintrbuf.spec1 & 0x0F;
			++doornr;
			break;
		case 4:
			if(spritetable[eintrbuf.art].id<8 && spec_gemz[spritetable[eintrbuf.art].id])
				break;  /* SPECIAL: Gems */
			addsprite(eintrbuf.art, eintrbuf.xpos<<5, eintrbuf.ypos<<5);
			break;
		}
		--dx;
	}

	fclose(fhndl);

	for(dx=rwx; dx<rwx+rww; dx++)
		for(dy=rwy; dy<rwy+rwh; dy++)
		{
			drawblock(dx, dy, room[dx][dy]);
		}

	offscr2win(rwx, rwy, rww, rwh);

	return 0;
}
