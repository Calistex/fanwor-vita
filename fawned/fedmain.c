/* ******* fedmain.c - Hauptroutinen ******* */

#include <SDL.h>

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>

#include "feddefs.h"
#include "fwdefs.h"
#include "fedinit.h"
#include "fedgraf.h"
#include "fedkeys.h"
#include "feddisk.h"
#include "level.h"
#include "fwdata.h"


/* **Variablen:** */
int mausx, mausy, mausk;        /* Zur Mausabfrage */
int key;                        /* Zur Tastaturabfrage */

int difgndnum;

SDL_Surface *sdlscrn;
SDL_Surface *offscrn;
SDL_Surface *groundgfx;
SDL_Surface *spritegfx;
SDL_Surface *fontgfx;


int endeflag=FALSE;                     /* TRUE=Spiel verlassen */

unsigned char sfeld[128][128];          /* Das Spielfeld */
unsigned char ffeld[128][128];
short rwx=0, rwy=0;                     /* Ab diesem Planquadrat beginnt das Fenster */
unsigned short rww=16, rwh=11;          /* Aktuelle Fenstergroesse (in Planquadraten) */
unsigned short r_width=16, r_height=11; /* Raumausmasse */

unsigned char acttile=0;		/* Ausgewaehlter Bodentyp */
short smodus=0;				/* 0=Feld editieren; 1=Typ auswaehlen */
short tmodus=1;

int en_anz=0;                           /* Anzahl der Eintraege */
LEVEL_EINTRAG en[256];                  /* Sprites, etc. */


void printhelp(void)
{
	fprintf(stderr,"\nTastenkommandos:\n");
	fprintf(stderr," n : Loesche aktuelles Feld.\n");
	fprintf(stderr," l : Level laden.\n");
	fprintf(stderr," s : Level speichern\n");
	fprintf(stderr," 0 : Sprite-Loesch Modus\n");
	fprintf(stderr," 1 : Hintergrund-Setz Modus\n");
	fprintf(stderr," 2 : Sprite-Setz Modus\n");
	fprintf(stderr," 3 : Tuer-Setz Modus (geht noch nicht)\n");
	fprintf(stderr," 4 : Item-Setz Modus\n");
	fprintf(stderr," d : Zeige Level-Daten (witzlos)\n");
	fprintf(stderr," h : Diese Hilfe zeigen\n");
	fprintf(stderr," q : Programm beenden\n");
}


/* ***Die Hauptroutine*** */
int main(int argc, char *argv[])
{
	short dx,dy;		/* Dummyvariablen */
	//short dw,dh;
	SDL_Event event;

	setsprttabl();

	if (initGUI() != 0)
		return -1;

	if (initgraf() != 0)
	{
		fprintf(stderr, "Failed to init graphics\n");
		exitGUI();
		return -1;
	}

	/* Clear the playground */
	for (dy = 0; dy < 128; dy++)
	{
		for (dx=0; dx < 128; dx++)
		{
			sfeld[dx][dy]=1;
			ffeld[dx][dy]=0;
		}
	}

	fprintf(stderr,"\n    **** Welcome to FAWNED ****\n");

	if (argc == 2 && argv[1][0] != '-')
	{
		loadlevel(argv[1]);
	}

	drawoffscr(rwx, rwy, rww, rwh);
	drwindow();

	/* Change current directory to the room folder */
	if (chdir("../rooms/") != 0)
	{
		fprintf(stderr, "Can not change to rooms directory\n");
		return -1;
	}

	printhelp();

	/* Die Hauptschleife: */
	do
	{
		if( !SDL_PollEvent(&event) )  continue;

		mausk=SDL_GetMouseState(&mausx, &mausy);
		if (smodus == 0)
		{
			unsigned short destx=mausx/32+rwx, desty=mausy/32+rwy;
			fprintf(stderr,"\rX=%i Y=%i  ",destx, desty);
			if (mausk == SDL_BUTTON(1))  /* Ins Spielfeld geklickt? */
			{
				//SDL_Rect drc;
				int f, x,y;
				switch(tmodus)
				{
				case 0:
					for( f=0; f<en_anz; f++ )
						if( en[f].xpos==destx && en[f].ypos==desty
						                && (en[f].eintrtyp==1 || en[f].eintrtyp==2 || en[f].eintrtyp==4) )
						{
							x=en[f].xpos;
							y=en[f].ypos;
							if( f != en_anz-1 )
								memcpy(&en[f], &en[en_anz-1], sizeof(LEVEL_EINTRAG));
							--en_anz;
							drawoffscr(x,y,1,1);
							/*drc.x=(x-rwx)*32+wi.g_x;
							drc.y=(y-rwy)*32+wi.g_y;
							drc.w=drc.g_h=32;*/
							drwindow();
						}
					break;
				case 1:
					sfeld[destx][desty]=acttile;
					if(acttile>=20) ffeld[destx][desty]|=FWALL;
					else ffeld[destx][desty]&=~FWALL;
					drawoffscr(destx, desty, 1, 1);
					/*drc.g_x=((mausx-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mausy-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;*/
					drwindow();
					break;
				case 2:                         /* Sprite */
					x=0;
					for( f=0; f<en_anz; f++ )
						if( en[f].xpos==destx && en[f].ypos==desty) x=1;
					if(x) break;
					en[en_anz].eintrtyp=1;
					en[en_anz].art=acttile;
					en[en_anz].xpos=destx;
					en[en_anz].ypos=desty;
					++en_anz;
					drawoffscr(destx, desty, 1, 1);
					/*drc.g_x=((mausx-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mausy-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;*/
					drwindow();
					break;
				case 3:                             /* Door */
#if 0
					x=0;
					for( f=0; f<en_anz; f++ )
						if( en[f].xpos==destx && en[f].ypos==desty) x=1;
					if(x) break;
					en[en_anz].eintrtyp=2;
					en[en_anz].xpos=destx;
					en[en_anz].ypos=desty;
					itoa(destx, doordlg[DXPOS].ob_spec.free_string, 10);
					itoa(desty, doordlg[DYPOS].ob_spec.free_string, 10);
					form_center(doordlg, &dx, &dy, &dw, &dh);
					form_dial(FMD_START, dx, dy, 0, 0, dx, dy, dw, dh);
					objc_draw(doordlg, 0, 8, dx, dy, dw, dh);
					form_do(doordlg, 0);
					form_dial(FMD_FINISH, dx, dy, 0, 0, dx, dy, dw, dh);
					doordlg[DOKAY].ob_state=NORMAL;
					en[en_anz].art=atoi(doordlg[DTYPE].ob_spec.tedinfo->te_ptext);
					dx=atoi(doordlg[DROOMX].ob_spec.tedinfo->te_ptext);
					dy=atoi(doordlg[DROOMY].ob_spec.tedinfo->te_ptext);
					if(dx>=16 || dy>=16)
					{
						form_alert(1,"[3][Fehler: Zielraum-|koordinaten zu|gross (max 16).][Abbruch]");
						break;
					}
					en[en_anz].spec1=(dx<<4)|dy;
					en[en_anz].xl=atoi(doordlg[DDXPOS].ob_spec.tedinfo->te_ptext);
					en[en_anz].yl=atoi(doordlg[DDYPOS].ob_spec.tedinfo->te_ptext);
					++en_anz;
					drc.g_x=((mausx-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mausy-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;
#endif
					drwindow();
					break;
				case 4:                         /* Item */
					x=0;
					for( f=0; f<en_anz; f++ )
						if( en[f].xpos==destx && en[f].ypos==desty) x=1;
					if(x) break;
					en[en_anz].eintrtyp=4;
					en[en_anz].art=acttile;
					en[en_anz].xpos=destx;
					en[en_anz].ypos=desty;
					++en_anz;
					drawoffscr(destx, desty, 1, 1);
					/*drc.g_x=((mausx-wi.g_x)&0xFFE0)+wi.g_x;
					drc.g_y=((mausy-wi.g_y)&0xFFE0)+wi.g_y;
					drc.g_w=drc.g_h=32;*/
					drwindow();
					break;
				}
			}
		}
		else
		{
			unsigned short desttile;
			desttile=mausy/32*rww+mausx/32;
			if(tmodus==1 && desttile>=difgndnum) desttile=0;
			if(tmodus==2)
			{
				if(desttile>=DIFSPRTNUM) desttile=3;
			}
			if(tmodus==4)
			{
				if(desttile>=DIFSPRTNUM) desttile=0;
			}
			fprintf(stderr,"\rTyp=%i  ",desttile);
			if(mausk==SDL_BUTTON(1))
			{
				acttile=desttile;
				mausk=SDL_BUTTON(3);	/* Modus wechseln (s.u.) */
			}
		}

		if( (tmodus==1 || tmodus==2 || tmodus==4) && ( (mausk==SDL_BUTTON(3))
		                || (event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_SPACE)) ) 	/* Modus wechseln? */
		{
			smodus^=1;
			while( SDL_GetMouseState(&mausx, &mausy) )
			{
				SDL_PumpEvents();
			};
			if(smodus==0)
			{
				fprintf(stderr, "\nSpielfeld editieren!\n");
			}
			else
			{
				switch(tmodus)
				{
				case 1:
					fprintf(stderr, "\nBodentyp auswaehlen!\n");
					break;
				case 2:
					fprintf(stderr, "\nSelect Sprite!\n");
					break;
				case 4:
					fprintf(stderr, "\nSelect Item!\n");
					break;
				}
			}
			drawoffscr(rwx, rwy, rww, rwh);
			drwindow();
		}

		if(event.type==SDL_QUIT)
			endeflag=TRUE;

		if(event.type==SDL_KEYDOWN)
			handle_keypress(event.key.keysym.sym);

	}
	while( !endeflag );


	exitGUI();

	fprintf(stderr,"\nUnd tschuess!\n");

	return(0);
}
