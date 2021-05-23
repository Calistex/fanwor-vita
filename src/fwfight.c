/*************************************************************/
/* fwfight.c  -  Handle the enemies movement, sword etc.     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See fwsource.txt for more information                     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <stdlib.h>

#include "fwmain.h"
#include "fwgui.h"
#include "fwgraf.h"
#include "fwreact.h"
#include "fwdata.h"
#include "fwmusic.h"
#include "fwfight.h"

#ifdef SOZOBON
#pragma -CUS
#endif

/* ***Move enemies*** */
void setenmys(void)
{
	register short i;
	register SPRT *aktsprt;
	short fl = 0;

	for (i = 1; i < spritenr; i++)
	{
		aktsprt=&sprites[i];
		if(aktsprt->class!=CENMY || aktsprt->hp<0)  continue;
		aktsprt->oldx=aktsprt->x;
		aktsprt->oldy=aktsprt->y;
		aktsprt->oldxpos=aktsprt->xpos;
		aktsprt->oldypos=aktsprt->ypos;
		switch(aktsprt->movement)
		{
		case 1:
		case 2:
			if(aktsprt->counter&1)  break;
		case 5:
			switch(aktsprt->directn)
			{
			case 1:
				if( aktsprt->y>0 )  aktsprt->y-=2;
				fl=!(aktsprt->y & 0x01F);
				break;
			case 2:
				if( aktsprt->x<r_width*32-32 )  aktsprt->x+=2;
				fl=!(aktsprt->x & 0x01F);
				break;
			case 3:
				if( aktsprt->y<r_height*32-32 )  aktsprt->y+=2;
				fl=!(aktsprt->y & 0x01F);
				break;
			case 4:
				if( aktsprt->x>0 )  aktsprt->x-=2;
				fl=!(aktsprt->x & 0x01F);
				break;
			}
			if (fl)
			{
				aktsprt->xpos=aktsprt->x/32;
				aktsprt->ypos=aktsprt->y/32;
				if ((rand()&3) == 1)
					aktsprt->directn=(rand()&3)+1;
				while( (aktsprt->directn==1 && (aktsprt->ypos<=0 || (ffield[aktsprt->xpos][aktsprt->ypos-1]&FWALL)) )
				        || (aktsprt->directn==2 && (aktsprt->xpos>=r_width-1 || (ffield[aktsprt->xpos+1][aktsprt->ypos]&FWALL)) )
				        || (aktsprt->directn==3 && (aktsprt->ypos>=r_height-1 || (ffield[aktsprt->xpos][aktsprt->ypos+1]&FWALL)) )
				        || (aktsprt->directn==4 && (aktsprt->xpos<=0 || (ffield[aktsprt->xpos-1][aktsprt->ypos]&FWALL)) )   )
				{
					aktsprt->directn=(rand()&3)+1;
				}
			}
			break;
		case 3:
			switch(aktsprt->directn)
			{
			case 1:
				if( aktsprt->y>2 )
					aktsprt->y-=rand()&2;
				else
					aktsprt->directn=3;
				if((aktsprt->counter&1) && aktsprt->x>1 && aktsprt->x<r_width*32-33)
					aktsprt->x-=(rand()&2)-1;
				break;
			case 2:
				if( aktsprt->x<r_width*32-34 )
					aktsprt->x+=rand()&2;
				else
					aktsprt->directn=4;
				if((aktsprt->counter&1) && aktsprt->y>1 && aktsprt->y<r_height*32-33)
					aktsprt->y-=(rand()&2)-1;
				break;
			case 3:
				if( aktsprt->y<r_height*32-34 )
					aktsprt->y+=rand()&2;
				else
					aktsprt->directn=1;
				if((aktsprt->counter&1) && aktsprt->x>1 && aktsprt->x<r_width*32-33)
					aktsprt->x-=(rand()&2)-1;
				break;
			case 4:
				if( aktsprt->x>2 )
					aktsprt->x-=rand()&2;
				else
					aktsprt->directn=2;
				if((aktsprt->counter&1)&& aktsprt->y>1 && aktsprt->y<r_height*32-33)
					aktsprt->y-=(rand()&2)-1;
				break;
			}
			if( (rand()&15)==1 )
				aktsprt->directn=(rand()&3)+1;
			break;
		case 4:
			if( aktsprt->counter > 5 )
			{
				if(aktsprt->step>0 && aktsprt->y>4) aktsprt->y-=2;
				if(aktsprt->step<0 && aktsprt->y<r_height*32-36) aktsprt->y+=2;
				switch(aktsprt->directn)
				{
				case 1:
					if( aktsprt->y>1 )  --aktsprt->y;
					break;
				case 2:
					if( aktsprt->x<r_width*32-33 )  ++aktsprt->x;
					break;
				case 3:
					if( aktsprt->y<r_height*32-33 )  ++aktsprt->y;
					break;
				case 4:
					if( aktsprt->x>1 )  --aktsprt->x;
					break;
				}
			}
			else if( (rand()&7)==1 )  aktsprt->directn=(rand()&3)+1;
			break;
		}

		aktsprt->counter+=aktsprt->step;
		if (aktsprt->counter <= 0)
			aktsprt->step = 1;
		if (aktsprt->counter >= aktsprt->maxcnter)
			aktsprt->step = -1;
		aktsprt->anim = aktsprt->counter/10;
		aktsprt->xpos = aktsprt->x/32;
		aktsprt->ypos = aktsprt->y/32;
	}
}


/* *** Show a sword attack *** */
void sword_blow(void)
{
	unsigned short dx[3], dy[3];
	register short i, j;
	register SPRT *aktsprt;
	//int redraw_gold;
	short weapnr;

	aktsprt=&sprites[0];
	if ((aktsprt->x < 32 && aktsprt->directn == 4)
	    || (aktsprt->x > r_width*32-63 && aktsprt->directn == 2)
	    || (aktsprt->y < 32 && aktsprt->directn == 1)
	    || (aktsprt->y > r_height*32-63 && aktsprt->directn == 3))
		return;

	sound_play(S_SWORD);

	sprites[0].anim=0;

	switch (sprites[0].directn)   /* Prepare the offset table */
	{
	case 1:
		dx[0]=dx[1]=dx[2]=0;
		dy[0]=-22;
		dy[1]=-24;
		dy[2]=-22;
		break;
	case 2:
		dx[0]=22;
		dx[1]=28;
		dx[2]=22;
		dy[0]=dy[1]=dy[2]=0;
		break;
	case 3:
		dx[0]=dx[1]=dx[2]=0;
		dy[0]=26;
		dy[1]=32;
		dy[2]=26;
		break;
	case 4:
		dx[0]=-24;
		dx[1]=-30;
		dx[2]=-24;
		dy[0]=dy[1]=dy[2]=0;
		break;
	default:
		puts("Bad direction!\n");
		exit(-1);
	}

	weapnr=spritenr;
	sprites[weapnr].class=CWEAPN;
	sprites[weapnr].index=2;
	sprites[weapnr].x=sprites[0].x+dx[0];
	sprites[weapnr].y=sprites[0].y+dy[0];
	sprites[weapnr].xpos=sprites[weapnr].x>>5;
	sprites[weapnr].ypos=sprites[weapnr].y>>5;
	sprites[weapnr].hp=spritetable[sprites[weapnr].index].hp;
	sprites[weapnr].directn=sprites[0].directn;
	sprites[weapnr].anim=0;
	j=0;
	for(i=0; i<sprites[weapnr].index; i++)
		j+=spritetable[i].animnr[0]+spritetable[i].animnr[1]+spritetable[i].animnr[2]+spritetable[i].animnr[3];
	sprites[weapnr].grfyoffset[0]=j*32;
	j+=spritetable[sprites[weapnr].index].animnr[0];
	sprites[weapnr].grfyoffset[1]=j*32;
	j+=spritetable[sprites[weapnr].index].animnr[1];
	sprites[weapnr].grfyoffset[2]=j*32;
	j+=spritetable[sprites[weapnr].index].animnr[2];
	sprites[weapnr].grfyoffset[3]=j*32;
	++spritenr;

	for(i=0; i<12; i++)
	{
		sprites[weapnr].oldx=sprites[weapnr].x;
		sprites[weapnr].oldy=sprites[weapnr].y;
		sprites[weapnr].oldxpos=sprites[weapnr].xpos;
		sprites[weapnr].oldypos=sprites[weapnr].ypos;
		sprites[weapnr].x=sprites[0].x+dx[i/4];
		sprites[weapnr].y=sprites[0].y+dy[i/4];
		sprites[weapnr].xpos=sprites[weapnr].x>>5;
		sprites[weapnr].ypos=sprites[weapnr].y>>5;
		drawsprites();
		if(i==5)
			for(j=1; j<spritenr; j++)
			{
				aktsprt=&sprites[j];
				if (aktsprt->class != CENMY)
					continue;
				if (aktsprt->hp > 0
				    && ( (aktsprt->x+31 > sprites[0].x && aktsprt->x < sprites[0].x+31
				          && ((sprites[0].directn == 1 && aktsprt->y+31 > sprites[0].y-31 && aktsprt->y<sprites[0].y)
				              || (sprites[0].directn == 3 && aktsprt->y > sprites[0].y && aktsprt->y < sprites[0].y+62)))
				        || (aktsprt->y+31>sprites[0].y && aktsprt->y<sprites[0].y+31
				            && ((sprites[0].directn == 2 && aktsprt->x > sprites[0].x && aktsprt->x < sprites[0].x+62)
				                ||(sprites[0].directn == 4 && aktsprt->x+30 > sprites[0].x-31 && aktsprt->x < sprites[0].x))) ) )
				{
					aktsprt->hp-=2;
				}
				if (aktsprt->hp <= 0)
				{
					aktsprt->hp=0;
					aktsprt->index=1;
					aktsprt->directn=1;
					aktsprt->anim=0;
					aktsprt->movement=0;
					aktsprt->grfyoffset[0]=8*32;
					aktsprt->counter=0;
					aktsprt->maxcnter=9;
				}
			}
		setenmys();
		waitms(20);
	}

	delsprite(weapnr);

	drawsprites();

	waitms(32);

	i=0;
	while(i++<spritenr)
		if(sprites[i].class==CENMY && sprites[i].hp==0)	/* Enemy killed? */
		{
			/*  if(gold<992)
			     gold+=rand() & 3; else gold=999;*/	/* Enlarge gold */
			sprites[i].hp=-1;
			delsprite(i);
			--i;
			/*    redraw_gold=TRUE;*/
		}

	/* if(redraw_gold)   write_hp_gold();*/		/* Redraw gold information */

}


#if 0
/* ***Big enemy explosion:*** */
void megaexplosion(void)
{
	WORD i, j;

	for (i = 0; i <= 4; i++)
	{
		for(j=0; j<=12; j++)  Vsync();
		sprite(badguy_x, badguy_y, 16, phys, explosion, badbuf[0]);
		sprite(badguy_x+16, badguy_y, 16, phys, explosion, badbuf[1]);
		sprite(badguy_x, badguy_y+16, 16, phys, explosion, badbuf[2]);
		sprite(badguy_x+16, badguy_y+16, 16, phys, explosion, badbuf[3]);
		for(j=0; j<=16; j++)  Vsync();
		clrspr(badbuf[3]);
		clrspr(badbuf[2]);
		clrspr(badbuf[1]);
		clrspr(badbuf[0]);
	}
}
#endif


#if 0
/* ***Check if a big enemy has been hurt*** */
BOOLEAN badguyhurt(void)
{
	if(
	        (badguy_x+30>hero.x && badguy_x<hero.x+15 &&
	         (
	          (hero.directn==1 && badguy_y+45>hero.y && badguy_y<hero.y)
	          ||(hero.directn==3 && badguy_y+15>hero.y && badguy_y<hero.y+30)
	         )
	        )
	        ||
	        (badguy_y+31>hero.y && badguy_y<hero.y+15 &&
	         (
	          (hero.directn==2 && badguy_x+15>hero.x && badguy_x<hero.x+30)
	          ||(hero.directn==4 && badguy_x+45>hero.x && badguy_x<hero.x)
	         )
	        )
	)
		return(TRUE);

	return(FALSE);
}
#endif
