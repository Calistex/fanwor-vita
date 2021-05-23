/* ******* fedgraf.c ******** */

#include "fedmain.h"
#include "feddefs.h"
#include "fwdefs.h"
#include "fwdata.h"
#include "sdlgui.h"

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


/* ***Palette setzen*** */
void setpal(SDL_Palette *palette)
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
					i=(r<<5)|(g<<2)|b;
					colors[i].r=r<<5;
					colors[i].g=g<<5;
					colors[i].b=b<<6;
				}
	}
	SDL_SetColors(sdlscrn,colors,0,ncolors);

}



/* *** Draw a ground block into the offscreen *** */
void drawblk(short xpos, short ypos, short num)
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


/* *** Teil des Offscreens neu zeichnen *** */
void drawoffscr(short ox, short oy, short ow, short oh)
{
	short i, j;
	SDL_Rect rs, rd;

	for(i=ox; i<ox+ow; i++)
		for(j=oy; j<oy+oh; j++)
		{
			drawblk(i, j, sfeld[i][j]);
		}
	for(i=0; i<en_anz; i++)
	{
		if( (en[i].eintrtyp==1 || en[i].eintrtyp==4) && (short)en[i].xpos>=ox && (short)en[i].ypos>=oy
		                && (short)en[i].xpos<ox+ow && (short)en[i].ypos<oy+oh )
		{
			rs.x=0;
			rs.y=0;
			for(j=0; j<en[i].art; j++)
				rs.y+=spritetable[j].animnr[0]+spritetable[j].animnr[1]+spritetable[j].animnr[2]+spritetable[j].animnr[3];
			rs.y*=32;
			rs.w=32;
			rs.h=32;
			rd.x=(en[i].xpos-rwx)*32;
			rd.y=(en[i].ypos-rwy)*32;
			rd.w=32;
			rd.h=32;
			SDL_BlitSurface(spritegfx, &rs, offscrn, &rd);
		}

	}

}



/* ***Grafik im Fenster aufbauen*** */
void drwindow()
{
	int i;
	SDL_Rect rs, rd;

	/* lock surface */

	if(smodus==0)  /* Editiermodus */
	{
		SDL_BlitSurface(offscrn, NULL, sdlscrn, NULL);
		for(i=0; i<en_anz; i++)
		{
			if( en[i].eintrtyp==2 && (short)en[i].xpos>=rwx && (short)en[i].ypos>=rwy
			                && (short)en[i].xpos<rwx+rww && (short)en[i].ypos<rwy+rwh )
			{
				rd.x = (en[i].xpos - rwx) * 32;
				rd.y = (en[i].ypos - rwy) * 32;
				rd.w = 8;
				rd.h = 16;
				SDL_FillRect(sdlscrn, &rd, SDL_MapRGB(sdlscrn->format, 255, 128, 255));
				SDLGui_Text(rd.x, rd.y, "D");
			}
		}
	}
	else  /* Auswahlmodus */
	{
		short dcx, dcy, i;
		switch(tmodus)
		{
		case 1:
			rs.x=0;
			rs.w=32;
			for(dcy=0; dcy<(short)rwh; dcy++)
				for(dcx=0; dcx<(short)rww; dcx++)
				{
					if(dcy*rww+dcx<difgndnum)
						rs.y=(dcy*rww+dcx)*32;
					else rs.y=0;
					rs.h=32;
					rd.x=dcx*32;
					rd.y=dcy*32;
					rd.w=rd.h=32;
					if(dcy*rww+dcx<difgndnum)
						SDL_BlitSurface(groundgfx, &rs, sdlscrn, &rd);
					else
						SDL_FillRect(sdlscrn, &rd, SDL_MapRGB(sdlscrn->format, 0, 0, 0) );
				}
			break;
		case 2:
		case 4:
			rs.x=0;
			rs.w=32;
			for(dcy=0; dcy<(short)rwh; dcy++)
				for(dcx=0; dcx<(short)rww; dcx++)
				{
					int index=dcy*rww+dcx;
					rs.y=0;
					if(index<DIFSPRTNUM)
						for(i=0; i<index; i++)
							rs.y+=spritetable[i].animnr[0]+spritetable[i].animnr[1]+spritetable[i].animnr[2]+spritetable[i].animnr[3];
					rs.y*=32;
					rs.h=32;
					rd.x=dcx*32;
					rd.y=dcy*32;
					rd.w=rd.h=32;
					if(index<DIFSPRTNUM)
					{
						SDL_FillRect(sdlscrn, &rd, SDL_MapRGB(sdlscrn->format, 192, 192, 192) );
						SDL_BlitSurface(spritegfx, &rs, sdlscrn, &rd);
					}
					else
						SDL_FillRect(sdlscrn, &rd, SDL_MapRGB(sdlscrn->format, 0, 0, 0) );
				}
			break;
		}
	}

	/* Redraw the status bar */
	rd.x = 0;
	rd.y = sdlscrn->h - 32;
	rd.w = sdlscrn->w;
	rd.h = 32;
	SDL_FillRect(sdlscrn, &rd, SDL_MapRGB(sdlscrn->format, 0, 0, 0) );

	/* Refresh screen */
	SDL_UpdateRect(sdlscrn, 0,0, 0,0);

	/* unlock surface */
}
