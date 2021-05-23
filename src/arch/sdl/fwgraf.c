/*************************************************************/
/* fwgraf.c - graphical output routines of Fanwor            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>

#include "fwmain.h"
#include "fwguiini.h"
#include "fwgui.h"
#include "loadpng.h"
#include "fwgraf.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/* **Variables:** */
SDL_Surface *sdlscrn;
SDL_Surface *offscrn;
SDL_Surface *groundgfx;
SDL_Surface *spritegfx;
SDL_Surface *fontgfx;


/**
 * Create palette
 */
static void setpal(SDL_Palette *palette)
{
	static SDL_Color *colors=NULL;
	int ncolors=256;

	if(colors==NULL)
		colors=(SDL_Color *)malloc(ncolors*sizeof(SDL_Color));

	if(palette)
	{
		ncolors=palette->ncolors;
		memcpy(colors, palette->colors, ncolors*sizeof(SDL_Color));
	}
	else
	{
		int r, g, b, i;
		ncolors=256;
		for(r=0; r<8; ++r)
			for(g=0; g<8; ++g)
				for(b=0; b<4; ++b)
				{
					i = (r<<5)|(g<<2)|b;
					colors[i].r = r<<5;
					colors[i].g = g<<5;
					colors[i].b = b<<6;
				}
	}
	SDL_SetColors(sdlscrn,colors,0,ncolors);

}


/**
 * Draw the hitpoints and amount of gold
 */
void write_hp_gold(void)
{
	SDL_Rect rs, rd;

	rs.x=0;
	rs.y=16*('0'+sprites[0].hp%10);
	rs.w=rs.h=16;
	rd.x=112;
	rd.y=360;
	rd.w=rd.h=16;
	SDL_BlitSurface(fontgfx, &rs, sdlscrn, &rd);
	rs.y=16*('0'+sprites[0].hp/10);
	rd.x-=16;
	SDL_BlitSurface(fontgfx, &rs, sdlscrn, &rd);
	rd.w=32;
	SDL_UpdateRects(sdlscrn, 1, &rd);
}


/**
 * Draw window using the offscreen bitmap
 */
static void drwindow(SDL_Rect *xywh)
{
	SDL_BlitSurface(offscrn, xywh, sdlscrn, xywh);
	SDL_UpdateRects(sdlscrn, 1, xywh);
}


/* ***Map a offscreen section to the window*** */
void offscr2win(short xpos, short ypos, short width, short height)
{
	SDL_Rect xywh;

	xywh.x=(xpos-rwx)*32;
	xywh.y=(ypos-rwy)*32;
	xywh.w=width*32;
	xywh.h=height*32;

	drwindow(&xywh);
}


/* *** Draw a ground block into the offscreen *** */
void drawblock(short xpos, short ypos, short num)
{
	SDL_Rect rs, rd;

	rs.x=0;
	rs.y=num*32;
	rs.w=32;
	rs.h=32;
	rd.x=(xpos-rwx)*32;
	rd.y=(ypos-rwy)*32;
	rd.w=32;
	rd.h=32;
	SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
}


/* ***Clear a sprite from the screen*** */
void clearsprite(short nr)
{
	SDL_Rect rs, rd;
	int rwxkoor, rwykoor;
	register SPRT *aktsprt;
	char rwflag;

	rwxkoor=rwx<<5;
	rwykoor=rwy<<5;
	rwflag=0;
	aktsprt=&sprites[nr];

	/** Redraw background: **/
	rs.x=0;
	rs.w=32;
	rd.w=32;
	if(aktsprt->oldxpos>=rwx && aktsprt->oldypos>=rwy)
	{
		rs.y=room[aktsprt->oldxpos][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
		rs.h=32-(rs.y & 0x01F);
		rd.x=(aktsprt->oldx-rwxkoor)&0xFFE0;
		rd.y=aktsprt->oldy-rwykoor;
		rd.h=32-(rd.y & 0x01F);
		SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
		rwflag=1;
	}
	if(aktsprt->oldy&0x01F && aktsprt->oldxpos>=rwx && aktsprt->oldypos+1<rwy+rwh)
	{
		rs.y=room[aktsprt->oldxpos][aktsprt->oldypos+1]*32;
		rs.h=aktsprt->oldy & 0x01F;
		rd.x=(aktsprt->oldx-rwxkoor)&0xFFE0;
		rd.y=(aktsprt->oldy+32-rwykoor)&0xFFE0;
		rd.h=rs.h;
		SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
		rwflag=1;
	}
	if(aktsprt->oldx&0x01F && aktsprt->oldxpos+1<rwx+rww && aktsprt->oldypos>=rwy)
	{
		rs.y=room[aktsprt->oldxpos+1][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
		rs.h=32-(rs.y & 0x01F);
		rd.x=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
		rd.y=aktsprt->oldy-rwykoor;
		rd.h=rs.h;
		SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
		rwflag=1;
	}
	if((aktsprt->oldx&0x01F) && (aktsprt->oldy&0x01F) && aktsprt->oldxpos+1<rwx+rww && aktsprt->oldypos+1<rwy+rwh)
	{
		rs.y=room[aktsprt->oldxpos+1][aktsprt->oldypos+1]*32;
		rs.h=aktsprt->oldy & 0x01F;
		rd.x=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
		rd.y=(aktsprt->oldy+32-rwykoor)&0xFFE0;
		rd.h=rs.h;
		SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
		rwflag=1;
	}

	/** Jetzt ins Fenster kopieren: **/
	if(rwflag)
	{
		rs.x=aktsprt->oldx-rwxkoor;
		rs.y=aktsprt->oldy-rwykoor;
		rs.w=32;
		rs.h=32;
		SDL_BlitSurface(offscrn, &rs, sdlscrn, &rs);  /* Sprite zeichnen */
		SDL_UpdateRects(sdlscrn, 1, &rs);
	}

}



/* ***Draw the sprites*** */
void drawsprites(void)
{
	register int i;
	SDL_Rect rs, rd;
	int rwxkoor, rwykoor;
	register SPRT *aktsprt;
	char rdsprflags[256]; /* 0=Sprite muss nicht neu gezeichnet werden, 1=Sprite neu zeichnen */

	rwxkoor=rwx<<5;
	rwykoor=rwy<<5;

	/** Redraw background: **/
	rs.x=0;
	rs.w=32;
	rd.w=32;
	for(i=0; i<spritenr; i++)
	{
		aktsprt=&sprites[i];
		if(aktsprt->hp>=0 && aktsprt->xpos>=rwx && aktsprt->ypos>=rwy && aktsprt->xpos<rwx+rww && aktsprt->ypos<rwy+rwh )
		{
			if(aktsprt->oldxpos>=rwx && aktsprt->oldypos>=rwy)
			{
				rs.y=room[aktsprt->oldxpos][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
				rs.h=32-(rs.y & 0x01F);
				rd.x=(aktsprt->oldx-rwxkoor)&0xFFE0;
				rd.y=aktsprt->oldy-rwykoor;
				rd.h=32-(rd.y & 0x01F);
				SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
			}
			if(aktsprt->oldy&0x01F && aktsprt->oldxpos>=rwx && aktsprt->oldypos+1<rwy+rwh)
			{
				rs.y=room[aktsprt->oldxpos][aktsprt->oldypos+1]*32;
				rs.h=aktsprt->oldy & 0x01F;
				rd.x=(aktsprt->oldx-rwxkoor)&0xFFE0;
				rd.y=(aktsprt->oldy+32-rwykoor)&0xFFE0;
				rd.h=rs.h;
				SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
			}
			if(aktsprt->oldx&0x01F && aktsprt->oldxpos+1<rwx+rww && aktsprt->oldypos>=rwy)
			{
				rs.y=room[aktsprt->oldxpos+1][aktsprt->oldypos]*32+(aktsprt->oldy&0x01F);
				rs.h=32-(rs.y & 0x01F);
				rd.x=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
				rd.y=aktsprt->oldy-rwykoor;
				rd.h=rs.h;
				SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
			}
			if((aktsprt->oldx&0x01F) && (aktsprt->oldy&0x01F) && aktsprt->oldxpos+1<rwx+rww && aktsprt->oldypos+1<rwy+rwh)
			{
				rs.y=room[aktsprt->oldxpos+1][aktsprt->oldypos+1]*32;
				rs.h=aktsprt->oldy & 0x01F;
				rd.x=((aktsprt->oldx-rwxkoor)&0xFFE0)+32;
				rd.y=(aktsprt->oldy+32-rwykoor)&0xFFE0;
				rd.h=rs.h;
				SDL_BlitSurface(groundgfx, &rs, offscrn, &rd);
			}
			rdsprflags[i+1]=1;
		}
		else rdsprflags[i+1]=0;   /* If not within the window => do not redraw */
	}

	/** Now draw the sprites: **/
	for(i=0; i<spritenr; i++)
	{
		aktsprt=&sprites[i];
		if(rdsprflags[i+1])
		{
			rs.y=aktsprt->grfyoffset[aktsprt->directn-1]+(aktsprt->anim*32);
			rs.h=32;
			rd.x=aktsprt->x-rwxkoor;
			rd.y=aktsprt->y-rwykoor;
			rd.w=32;
			rd.h=32;
			if(rd.x<0 || rd.y<0 || rd.x+rd.w>(rww<<5) || rd.y+rd.h>(rwh<<5))
				continue;
			SDL_BlitSurface(spritegfx, &rs, offscrn, &rd);  /* Sprite zeichnen */
		}
	}

	/** Jetzt in den Bildschirm kopieren: **/
	for(i=0; i<spritenr; i++)
	{
		aktsprt=&sprites[i];
		if(!rdsprflags[i+1])
			continue; /* Nur wenn noetig: Spart einiges an CPU-Zeit */
		rs.x = min(aktsprt->x, aktsprt->oldx)-rwxkoor;
		rs.y = min(aktsprt->y, aktsprt->oldy)-rwykoor;
		rs.w = max(aktsprt->x, aktsprt->oldx)+32-rwxkoor-rs.x;
		rs.h = max(aktsprt->y, aktsprt->oldy)+32-rwykoor-rs.y;
		SDL_BlitSurface(offscrn, &rs, sdlscrn, &rs);  /* Sprite zeichnen */
		SDL_UpdateRects(sdlscrn, 1, &rs);
	}  /* for */

}


/* ***Show a picture and wait for a key*** */
int showpicture(char *name)
{
	char fullname[128];
	SDL_Surface *pic;
	SDL_Rect rs, rd;

	strcpy(fullname, "app0:graphics/");
	strcat(fullname, name);
	strcat(fullname, ".png");

	pic=LoadPNG(fullname);
	if( pic==NULL )
	{
		fprintf(stderr, "Could not load image:\n %s\n", SDL_GetError() );
		return 3;
	}

	setpal( pic->format->palette );

	rs.x=0;
	rs.y=0;
	rs.w=pic->w;
	rs.h=pic->h;
	rd.x=0;
	rd.y=0;
	rd.w=pic->w;
	rd.h=pic->h;

	SDL_BlitSurface(pic,&rs,sdlscrn,&rd);
	SDL_UpdateRects(sdlscrn, 1, &rd);

	do event_handler();
	while(flag_quit==FALSE && tdirectn<128);
	do event_handler();
	while(flag_quit==FALSE && tdirectn>=128);

	SDL_FreeSurface(pic);

	rd.x=0;
	rd.y=0;
	rd.w=rww*32;
	rd.h=(rwh+1)*32;
	SDL_FillRect(sdlscrn, &rd, SDL_MapRGB(sdlscrn->format, 0, 0, 0) );
	SDL_UpdateRects(sdlscrn, 1, &rd);

	setpal(spritegfx->format->palette);

	/* Repaint the status bar: */
	rd.x=0;
	rd.y=rwh*32;
	rd.w=rww*32;
	rd.h=32;
	SDL_FillRect(sdlscrn, &rd, SDL_MapRGB(sdlscrn->format, 0, 0, 0) );
	rs.x=0;
	rs.y=0;
	rs.w=rs.h=16;
	rd.x=64;
	rd.y=360;
	rd.w=rd.h=16;
	SDL_BlitSurface(fontgfx, &rs, sdlscrn, &rd);
	rs.y='='*16;
	rd.x+=16;
	SDL_BlitSurface(fontgfx, &rs, sdlscrn, &rd);
	rd.x=0;
	rd.y=rwh*32;
	rd.w=rww*32;
	rd.h=32;
	SDL_UpdateRects(sdlscrn, 1, &rd);

	return 0;
}
