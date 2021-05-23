/*************************************************************/
/* fwmain.c  -  The main() function and some others          */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See fwsource.txt for more information                     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <stdlib.h>
#include <string.h>

#include "fwdefs.h"
#include "fwdisk.h"
#include "fwguiini.h"
#include "fwgui.h"
#include "fwgraf.h"
#include "fwdata.h"
#include "fwreact.h"
#include "fwfight.h"
#include "fwmusic.h"
#include "fwmain.h"

int g_argc;
char **g_argv;

unsigned char tdirectn;            /* The direction the hero should walk to */
short alrdyhit;                    /* Firebutton held down? */
unsigned char room[128][128];      /* Room data */
unsigned char ffield[128][128];    /* Room flags */
char room_x, room_y;               /* Raumkoordinaten */
unsigned char roomnr;              /* Gruppennummer des Raumes */
short gold;                        /* Goldvorrat */

SPRT sprites[MAXSPRTNR];           /* The sprites on the screen */
short spritenr;                    /* The number of sprites */
short doornr;                      /* The amount of doors */
DOOR doors[MAXDOORNR];             /* The doors of the current room */
short rwx=0, rwy=0, rww=16, rwh=11;
short r_width=16, r_height=11;

/* The flags: */
char flag_ende;                  /* Game completed? */
char flag_quit;                  /* Quit the game? */



/* *** Remove a sprite from the list *** */
void delsprite(short nr)
{
	clearsprite(nr);
	if (nr < spritenr-1)
	{
		memcpy(&sprites[nr], &sprites[spritenr-1], sizeof(SPRT));
	}
	--spritenr;
}

/* *** Add a new sprite to the list *** */
void addsprite(short index, short x, short y)
{
	short dy, i;

	sprites[spritenr].class=spritetable[index].class;
	sprites[spritenr].index=index;

	sprites[spritenr].x=sprites[spritenr].oldx=x;
	sprites[spritenr].y=sprites[spritenr].oldy=y;
	sprites[spritenr].xpos=sprites[spritenr].oldxpos=x>>5;
	sprites[spritenr].ypos=sprites[spritenr].oldypos=y>>5;

	sprites[spritenr].hp=spritetable[index].hp;
	sprites[spritenr].directn=1;

	sprites[spritenr].anim=0;
	sprites[spritenr].step=1;
	sprites[spritenr].maxcnter=spritetable[index].animnr[sprites[spritenr].directn-1]*10-1;
	sprites[spritenr].counter=(rand()&7)+1;
	sprites[spritenr].movement=spritetable[index].movement;

	dy=0;
	for (i=0; i<index; i++)
		dy += spritetable[i].animnr[0]+spritetable[i].animnr[1]+spritetable[i].animnr[2]+spritetable[i].animnr[3];
	sprites[spritenr].grfyoffset[0]=dy*32;
	if (spritetable[index].animnr[1] > 0)
		dy += spritetable[index].animnr[0];
	sprites[spritenr].grfyoffset[1]=dy*32;
	dy+=spritetable[index].animnr[1];
	sprites[spritenr].grfyoffset[2]=dy*32;
	dy+=spritetable[index].animnr[2];
	sprites[spritenr].grfyoffset[3]=dy*32;
	++spritenr;
}


/**
 * Set variables to initial values
 */
static void init_vars(void)
{
	room_x = 7;
	room_y = 7;
	roomnr = 0;	/* Anfangswerte setzen */
	sprites[0].class = 0;
	sprites[0].index = 0;
	sprites[0].x = sprites[0].oldx = 304;
	sprites[0].y = sprites[0].oldy = 184;
	sprites[0].xpos = sprites[0].oldxpos = sprites[0].x/32;
	sprites[0].ypos = sprites[0].oldypos = sprites[0].y/32;
	sprites[0].hp = 20;
	sprites[0].directn = 3;
	sprites[0].anim = 0;
	sprites[0].step = 1;
	sprites[0].counter = 0;
	sprites[0].maxcnter = 19;
	sprites[0].grfyoffset[0] = 0;
	sprites[0].grfyoffset[1] = 64;
	sprites[0].grfyoffset[2] = 128;
	sprites[0].grfyoffset[3] = 192;
	spritenr = 1;
	gold = 0;
	flag_ende = FALSE;
}

/**
 * The main loop
 */
int main(int argc, char *argv[])
{
	g_argc=argc;
	g_argv=argv;

	setsprttabl();

	if (initGUI())
		return -1;

	if (sound_init())
	{
		alertdlg("Could not load\nthe sound samples");
	}

	mod_init();
	mod_play("app0:sounds/backgrnd.mod");

	if (initgraf())
	{
		alertdlg("Error while\ninitializing the gfx!");
		exitGUI();
		return -1;
	}
	open_window();

	do
	{
		showpicture("titel");

		init_vars();

		write_hp_gold();

		loadroom();			/* Load 1st room and draw it */

		do
		{

			sethero();
			setenmys();
			sprtcollision();
			drawsprites();

			event_handler();

			if (tdirectn > 127 && !alrdyhit)
			{
				sword_blow();        /* Schwerthieb */
				/*clearkeybuf();*/
				alrdyhit=TRUE;
			}
			else if (tdirectn < 128)
			{
				alrdyhit = FALSE;
			}

			if ((sprites[0].x==0 && tdirectn==4) || (sprites[0].x==r_width*32-32 && tdirectn==2)
			    || (sprites[0].y==0 && tdirectn==1) || (sprites[0].y==r_height*32-32 && tdirectn==3))
			{
				/* Raum verlassen? */
				short oldrw, oldrh;
				oldrw=r_width;
				oldrh=r_height;
				if(sprites[0].x<=0) --room_x;
				if(sprites[0].x>=r_width*32-32)  ++room_x;
				if(sprites[0].y<=0)  --room_y;
				if(sprites[0].y>=r_height*32-32)  ++room_y;
				loadroom();					/* Neuen Raum laden */
				if(sprites[0].x<=0)
				{
					sprites[0].x=sprites[0].oldx=oldrw*32-34;
					sprites[0].xpos=sprites[0].oldxpos=sprites[0].x/32;
				}
				if(sprites[0].x>=oldrw*32-32)
				{
					sprites[0].x=sprites[0].oldx=2;
					sprites[0].xpos=sprites[0].oldxpos=sprites[0].x/32;
				}
				if(sprites[0].y<=0)
				{
					sprites[0].y=sprites[0].oldy=oldrh*32-34;
					sprites[0].ypos=sprites[0].oldypos=sprites[0].y/32;
				}
				if(sprites[0].y>=oldrh*32-32)
				{
					sprites[0].y=sprites[0].oldy=2;
					sprites[0].ypos=sprites[0].oldypos=sprites[0].y/32;
				}
			}

			/*if(peoplenr)  testpeople();*/		/* Gib es Leute im Raum? */
			if(doornr)  testdoors();			/* Entered a door? */

		}
		while(sprites[0].hp>0 && !flag_quit); /* Bis keine Energie mehr bzw Happyend */

		if(sprites[0].hp==0)                  /* Game over */
		{
			/*redflashlight();*/
			showpicture("gameovr");
		}

		if(flag_ende)      /* Happy-End */
		{
			flag_quit=FALSE;
			sound_play(S_SUCCESS);
			showpicture("success");
			flag_quit=TRUE;
		}

	}
	while(!flag_quit);

	close_window();

	mod_stop();
	sound_deinit();

	deinitgraf();

	exitGUI();

	return 0;
}
