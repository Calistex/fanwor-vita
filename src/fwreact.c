/*************************************************************/
/* fwreact.c  -  React to doors, people, hero etc.           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include "fwdefs.h"
#include "fwmain.h"
#include "fwgui.h"
#include "fwgraf.h"
#include "fwdisk.h"
#include "fwdata.h"
#include "fwmusic.h"
#include "fwreact.h"


/* ** Variables: ** */
char spec_gemz[8];
static short spec_gemznr = 0;


/* ***Check if the player entered a door*** */
void testdoors(void)
{
	unsigned short i, dx, dy;

	dx=sprites[0].x & 0x01F;
	dy=sprites[0].y & 0x01F;
	if((sprites[0].directn==2 || sprites[0].directn==4) && dx>2 && dx<30)
		return;
	if((sprites[0].directn==1 || sprites[0].directn==3) && dy>2 && dy<30)
		return;

	dx=(sprites[0].x+16)>>5;
	dy=(sprites[0].y+16)>>5;
	for(i=0; i<doornr; i++)
	{
		if(
		   (sprites[0].directn==1 && dx==doors[i].x && dy-1==doors[i].y) ||
		   (sprites[0].directn==2 && dx+1==doors[i].x && dy==doors[i].y) ||
		   (sprites[0].directn==3 && dx==doors[i].x && dy+1==doors[i].y) ||
		   (sprites[0].directn==4 && dx-1==doors[i].x && dy==doors[i].y)
		  )
		{
			roomnr=doors[i].destnr;
			room_x=doors[i].destx;
			room_y=doors[i].desty;
			sprites[0].x=sprites[0].oldx=doors[i].exitx*32;
			sprites[0].xpos=sprites[0].oldxpos=sprites[0].x/32;
			sprites[0].y=sprites[0].oldy=doors[i].exity*32;
			sprites[0].ypos=sprites[0].oldypos=sprites[0].y/32;
			loadroom();                          /* New room */
			break;
		}
	}
}


/* *** Move the hero *** */
void sethero(void)
{
	register unsigned short dx1, dx2, dy1, dy2, d3;

	sprites[0].oldx=sprites[0].x;
	sprites[0].oldy=sprites[0].y;
	sprites[0].oldxpos=sprites[0].xpos;
	sprites[0].oldypos=sprites[0].ypos;

	dx1=(sprites[0].x+1) >> 5;
	dy1=(sprites[0].y+1) >> 5;
	dx2=(sprites[0].x+30) >> 5;
	dy2=(sprites[0].y+31) >> 5;

	/* Richtung auswerten + auf Hindernisse testen */
	switch(tdirectn)
	{
	case 1:
		d3=sprites[0].y>>5;
		if( !(ffield[dx1][d3]&FWALL) && !(ffield[dx2][d3]&FWALL) )
			sprites[0].y-=2;
		else
		{
			if( !(ffield[dx1][d3]&FWALL) ) sprites[0].x-=2;
			if( !(ffield[dx2][d3]&FWALL) ) sprites[0].x+=2;
		}
		break;
	case 2:
		d3=(sprites[0].x+31)>>5;
		if( !(ffield[d3][dy1]&FWALL) && !(ffield[d3][dy2]&FWALL) )
			sprites[0].x+=2;
		else
		{
			if( !(ffield[d3][dy1]&FWALL) ) sprites[0].y-=2;
			if( !(ffield[d3][dy2]&FWALL) ) sprites[0].y+=2;
		}
		break;
	case 3:
		d3=(sprites[0].y+32)>>5;
		if( !(ffield[dx1][d3]&FWALL) && !(ffield[dx2][d3]&FWALL) )
			sprites[0].y+=2;
		else
		{
			if( !(ffield[dx1][d3]&FWALL) ) sprites[0].x-=2;
			if( !(ffield[dx2][d3]&FWALL) ) sprites[0].x+=2;
		}
		break;
	case 4:
		d3=sprites[0].x>>5;
		if( !(ffield[d3][dy1]&FWALL) && !(ffield[d3][dy2]&FWALL) )
			sprites[0].x-=2;
		else
		{
			if( !(ffield[d3][dy1]&FWALL) ) sprites[0].y-=2;
			if( !(ffield[d3][dy2]&FWALL) ) sprites[0].y+=2;
		}
		break;
	}

	sprites[0].counter+=sprites[0].step;
	if(sprites[0].counter==0)  sprites[0].step=1;
	if(sprites[0].counter==sprites[0].maxcnter)  sprites[0].step=-1;
	if(tdirectn>=1 && tdirectn<=4)
	{
		sprites[0].anim=sprites[0].counter/10;
		sprites[0].directn=tdirectn;
	}

	sprites[0].xpos = sprites[0].x/32;
	sprites[0].ypos = sprites[0].y/32;
}


/* *** Does the player touch another sprite? *** */
void sprtcollision(void)
{
	register unsigned short i;
	static unsigned short regain=0;
	short redraw_hp=FALSE;
	static short fairyflag;

	/* SPECIAL: Fairy room */
	if(roomnr==0 && ((room_x==3 && room_y==4) || (room_x==9 && room_y==3)))
	{
		if(!fairyflag && (sprites[0].xpos>=5 && sprites[0].xpos<=10 && sprites[0].ypos==7))
		{
			sound_play(S_TAKEITEM);
			fairyflag=1;
			sprites[0].hp=20+spec_gemznr*10;
			if (sprites[0].hp >= 100)
				sprites[0].hp = 99;
			redraw_hp=TRUE;
		}
	}
	else fairyflag=0;

	for (i = 1; i < spritenr; i++)
	{
		if(   sprites[i].x+31>sprites[0].x && sprites[i].x<sprites[0].x+31
		                && sprites[i].y+31>sprites[0].y && sprites[i].y<sprites[0].y+31 )
		{
			switch(sprites[i].class)
			{
			case CENMY:
				if(regain || sprites[i].hp<=0)  continue;
				--sprites[0].hp;
				regain=8;
				redraw_hp=TRUE;
				break;
			case CITEM:
				sound_play(S_TAKEITEM);
				if(spritetable[sprites[i].index].id<8)     /* SPECIAL: Gem collected? */
				{
					spec_gemz[spritetable[sprites[i].index].id]=1;
					++spec_gemznr;
					if(spec_gemznr==8) flag_ende=flag_quit=TRUE; /* Game finished */
					sprites[0].hp=20+spec_gemznr*10;
					if(sprites[0].hp>=100) sprites[0].hp=99;
					redraw_hp=TRUE;
				}
				delsprite(i);
				--i;
				break;
			}

		}
	}

	if(regain)  --regain;
	if(redraw_hp)  write_hp_gold();		/* Hitpoints neu zeichnen */
}


/* *** Check if the player wants to talk to another person*** */
void testpeople(void)
{
#if 0
	short i;
	short tflag;
	unsigned short dhx1, dhx2, dhy1, dhy2, dpx, dpy;

	/* Testen, ob Spieler vor einer Person steht */
	dhx1=(sprites[0].x+1) >> 4;
	dhy1=(sprites[0].y+1) >> 4;
	dhx2=(sprites[0].x+15) >> 4;
	dhy2=(sprites[0].y+15) >> 4;
	for(i=1; i<spritenr; i++)
	{
		if(sprites[i].class!=CPEOPL) continue;
		tflag=0;
		dpx=(sprites[i].x) >> 4;
		dpy=(sprites[i].y) >> 4;
		switch(sprites[0].directn)  /* Do we touch someone? */
		{
		case 1:
			if( (dhx1==dpx || dhx2==dpx) && dhy1==dpy )
			{
				++sprites[0].y;
				tflag=TRUE;
			}
			break;
		case 2:
			if( (dhy1==dpy || dhy2==dpy) && dhx2==dpx )
			{
				--sprites[0].x;
				tflag=TRUE;
			}
			break;
		case 3:
			if( (dhx1==dpx || dhx2==dpx) && dhy2==dpy )
			{
				--sprites[0].y;
				tflag=TRUE;
			}
			break;
		case 4:
			if( (dhy1==dpy || dhy2==dpy) && dhx1==dpx )
			{
				++sprites[0].x;
				tflag=TRUE;
			}
			break;
		}
		if(tflag)  do_dial(sprites[i].spec);    /* Talk! */
	}
#endif
}
