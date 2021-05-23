/*************************************************************/
/* fwgui.c  -  Music and sound functions                     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                            */
/* See "fwsource.txt" for more information                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU General Public     */
/* License, version 2 or at your option any later version.   */
/* Please read the file "gpl.txt" for details.               */
/*************************************************************/

#include <string.h>
#include <osbind.h>
#include <falcon.h>
#include <aes.h>

#include "fwgui.h"
#include "fwguiini.h"  /* For xgetcookie() */


/* ########### MOD Definitionen und Strukturen ############ */

/* **Die defines von TeTra/Tetrax:** */
#define MOD_OK       0x4D00
/* MOD_NAK: Not okay, the command was understood but rejected. */
#define MOD_NAK      0x4D01
/* MOD_PLAY: Play the module, MOD_OK or MOD_NAK is replied. */
#define MOD_PLAY     0x4D02
/* MOD_STOP: Stop playing the module. */
#define MOD_STOP     0x4D03
/* MOD_LOAD: Load a module into the buffer, MOD_NAK is replied if
 something went wrong, MOD_OK if it is okay. msgbuf[3]+msgbuf[4]
 are a pointer to the module filename. */
#define MOD_LOAD     0x4D04
/* MOD_CLEAR: Stop module and clear the buffer. */
#define MOD_CLEAR    0x4D05

/* **Die defines von Paula (mind. V2.4!)** */
/* MP_ACK: Paula sagt OK! Das vorige Kommando ist verstanden
    worden. msg[7] enthaelt die Versionsnummer des Programms; 0x204 =
    v2.4 */
#define MP_ACK 0x4800
/* MP_NAK: Paula sagt NICHT OK! Das vorige Kommando ist
    nicht verstanden bzw. abgelehnt worden. msg[7] enthaelt die
    Versionsnummer des Programms; 0x204 = v2.4 */
#define MP_NAK 0x4801
/* MP_START: Funktion ist identisch mit VA_START. Nach
    Uebernahme des Kommandostrings schickt Paula aber eine MP_ACK
    (bzw. MP_NAK)-Nachricht an den Absender zurueck, und signalisiert
    so, dass der in der MP_START-Nachricht angegebene Speicherbereich
    anderweitig benutzt werden kann. */
#define MP_START 0x4802
/* MP_STOP: Stoppt Paula und gibt den fuer MODs und Playlisten
    allozierten Speicher frei. Das Fenster bleibt offen. Paula
    schickt MP_ACK zurueck. */
#define MP_STOP 0x4803
/* MP_SHUTDOWN: Stoppt und beendet Paula. Im Accessory-
    Modus wird nur der Speicher freigegeben und das Fenster
    geschlossen. Paula schickt MP_ACK zurueck. */
#define MP_SHUTDOWN 0x4804

/* **Die defines von Ultimate-Tracker:** */
#define  T_LOAD      900
#define  T_PLAY      901
#define  T_PAUSE     904
#define  T_STOP      905
#define  ID_TRACK    999


/* *** SPI Interface Strukturen *** */
typedef struct
{
	long magic;
	short mainvers;        /* SPI Definitionsversion */
	short spivers;         /* Version of the SPI - quite unimportant! */
	void *tbp;				/* Pointer to the basepage of the player */
	unsigned long support;	/* Bitfield of supported calls/modes */
	long plflags;			/* Bitfield of player-flags */
	long res1;
	char *cstr1;			/* Pointer to the 1st Copyrightstring */
	char *cstr2;			/*	  "		"  "  2nd	"		"	  */
	char *frqstr1;
	char *frqstr2;
	char *frqstr3;
	int (*modplay)(void *modaddr);		/* Play routine */
	int (*modstop)();		/* Stop routine */
	int (*modsetup)(short opcode, long subcode);	/* Configuration routine */
	int (*modpause)();		/* Pause */
	int (*modforward)();	/* Fast forward*/
	int (*modbackward)();	/* Fast backward */
	int (*samplay)();
} SPI;

typedef struct
{
	char		*p_lowtpa;
	char		*p_hitpa;
	SPI		*(*p_tbase)();
	long		p_tlen;
	char		*p_dbase;
	long		p_dlen;
	char		*p_bbase;
	long		p_blen;
	char		*p_dta;
	struct basep	*p_parent;
	char		*p_resrvd0;
	char		*p_env;
	char		p_resrvd1[80];
	char		p_cmdlin[128];
} SPI_BP;



/* ############## Variablen ############ */

/* PSG sounds, ready for Dosound() */
unsigned char swordsnd[]=
{
	7,0xDB, 6,15, 8,0, 9,0, 10,0, 11,1000&0xFF, 12,1000>>8,
	4,7, 5,119, 13,14, 10,16, 0xFF,20, 10,0, 13,1, 7,0xD8, 0xFF,0
};
unsigned char takeitemsnd[]=
{
	7,0xD9, 8,0, 10,0, 2,102, 3,1, 9,12, 0xFE,15, 2,239, 3,0,
	0xFE,45, 7,0xDB, 9,0, 0xFF,0
};


/* Flags zum Abspielmodus: */
short sndpsgflag=1;
short sndsamflag=0;
short sndmodflag=0;

/* Allgemeine Variablen: */
int samqueue[8]= {-1,-1,-1,-1,-1,-1,-1,-1};    /* Queue for the samples */
int sqanz=0;

int ptyp;               /* Playertyp: 0=keiner; 1=SPI; 2=TeTra; 3=Paula; 4=U-Tracker */
int pl_id;              /* AES-Id des Players */
int wmsgbf[8];          /* Nachrichtenbuffer zum Verschicken */

SPI *tspi;
short playflag=FALSE;
void *mod_addr=0L;
short aktchan=0;


#define SAM_ANZAHL 3
typedef struct
{
	char *name;
	void *start;           /* Anfangsadresse */
	void *end;             /* Endadresse */
	void *dosnd;           /* Alternativer DoSound */
} THSAMPLE;

THSAMPLE samples[SAM_ANZAHL]=
{
	"sword.raw",0L,0L,swordsnd,
	"takeitem.raw",0L,0L,takeitemsnd,
	"success.raw",0L,0L,takeitemsnd
};


/* ############# Funktionen ############### */


/* ***MOD-Player initialisieren*** */
int mod_init(void)
{
	short sfh;
	SPI_BP *spibp;
	int i;
	char *sname;
	unsigned long scookie;

	if( !xgetcookie('_SND', &scookie) ) scookie=0;

	for(i=0; i<=2; i++)
	{
		switch(i)
		{
		case 0:
			if( (scookie & 0x1E)!=0x1E ) continue;
			sname="sounds\\dsp.spi";
			break;
		case 1:
			if( (scookie & 0x2)!=0x2 ) continue;
			sname="sounds\\dma.spi";
			break;
		case 2:
			if( (scookie & 0x20)!=0x20 ) continue;
			sname="sounds\\xbios.spi";
			break;
		}
		if( (sfh=Fopen(sname, 0)) > 0L )	/* Check if the SPI exists */
		{
			Fclose(sfh);
			spibp=(SPI_BP *)Pexec(3, sname, "", "");	/* Load the SPI */
			if((long)spibp>0L)
			{
				Mshrink(spibp, spibp->p_tlen+spibp->p_dlen+spibp->p_blen+256L);
				tspi=spibp->p_tbase();					/* Init the TPI */
				if((long)tspi>0L)
				{
					tspi->tbp=spibp;
					ptyp=1;
					return(0);
				}
				else
				{
					/*form_alert(1, "[3][SPI init error][Ok]");*/
					tspi=0L;
					Mfree(spibp);
				}
			}
		}
	}

	pl_id=appl_find("TETRA   ");
	if(pl_id>=0)
	{
		ptyp=2;
		return(0);
	}

	pl_id=appl_find("TETRAX  ");
	if(pl_id>=0)
	{
		ptyp=2;
		return(0);
	}

	pl_id=appl_find("PAULA   ");
	if(pl_id>=0)
	{
		ptyp=3;
		return(0);
	}

	pl_id=appl_find("U_TRACK ");
	if(pl_id>=0)
	{
		ptyp=4;
		return(0);
	}

	ptyp=0;
	return(-1);
}

/* ***Auf das Okay des Players warten*** */
int wait4plok()
{
	int i, mok, mnak;

	if(ptyp==2)
	{
		mok=MOD_OK;
		mnak=MOD_NAK;
	}
	if(ptyp==3)
	{
		mok=MP_ACK;
		mnak=MP_NAK;
	}

	i=0;
	do
	{
		evnt_mesag(msgbuf);
		if( msgbuf[0]==mnak || i++>10)  return(-1);
		if( msgbuf[0]!=mok )  mesages();
	}
	while( msgbuf[0]!=mok );

	return(0);
}


/* ***MOD spielen*** */
int mod_play(const char *mname)
{
	int i;
	char fullname[255];

	fullname[0]='A'+Dgetdrv();
	fullname[1]=':';
	Dgetpath(&fullname[2], 0);
	if(fullname[strlen(fullname)-1]!='\\')  strcat(fullname, "\\");
	strcat(fullname, mname);

	wmsgbf[1]=ap_id;
	wmsgbf[2]=0;

	if( !sndmodflag )  return(1);

	switch(ptyp)
	{
	case 1:
		if(tspi)
		{
			int fhndl;
			long flength;
			if(playflag && mod_addr)	/* Already playing? */
				tspi->modstop();		/* Yes: Stop first */
			fhndl=Fopen(fullname, 0);
			if(fhndl<0) return(fhndl);
			flength=Fseek(0L, fhndl, 2);     /* Get file size */
			Fseek(0L, fhndl, 0);
			mod_addr=(char *)Mxalloc(flength, 0);  /* Speicher reservieren */
			if( ((signed long)mod_addr)==-32L )  mod_addr=(char *)Malloc(flength);
			if( ((signed long)mod_addr)<=0L )  return(mod_addr);
			Fread(fhndl, flength, mod_addr);
			Fclose(fhndl);
			tspi->modplay(mod_addr);	/* Start playing */
		}
		break;
	case 2:  /* Tetra */
		wmsgbf[0]=MOD_LOAD;
		*(char **)&wmsgbf[3]=fullname;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		if( wait4plok() ) return(-1);
		wmsgbf[0]=MOD_PLAY;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		if( wait4plok() ) return(-1);
		break;
	case 3:  /* Paula */
		wmsgbf[0]=MP_START;
		*(char **)&wmsgbf[3]=fullname;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		if( wait4plok() ) return(-1);
		break;
	case 4:  /* Ultimate Tracker */
		wmsgbf[0]=ID_TRACK;
		wmsgbf[1]=T_LOAD;
		*(char **)&wmsgbf[2]=fullname;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		wmsgbf[1]=T_PLAY;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		break;
	default:
		return(-1);
	}

	playflag=TRUE;

	return(0);
}

/* ***MOD-Wiedergabe beenden*** */
int mod_stop()
{
	wmsgbf[1]=ap_id;
	wmsgbf[2]=0;

	switch(ptyp)
	{
	case 1:  /* SPI */
		if(tspi && playflag)
		{
			tspi->modstop();
			Mfree(mod_addr);
			mod_addr=0L;
		}
		break;
	case 2:  /* TeTra */
		wmsgbf[0]=MOD_STOP;
		if( !appl_write(pl_id, 16, wmsgbf) ) return(-1);
		if( wait4plok() ) return(-1);
		wmsgbf[0]=MOD_CLEAR;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		if( wait4plok() ) return(-1);
		break;
	case 3:  /* Paula */
		wmsgbf[0]=MP_STOP;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		if( wait4plok() ) return(-1);
		break;
	case 4:  /* Ultimate Tracker */
		wmsgbf[0]=ID_TRACK;
		wmsgbf[1]=T_STOP;
		if( !appl_write(pl_id, 16, wmsgbf) )  return(-1);
		break;
	default:
		return(-1);
	}

	playflag=FALSE;

	return(0);
}



/* ***Samplesounds laden:*** */
long sound_init(void)
{
	long flength;
	int fhndl, i;
	char *samadr;
	char samname[22];

	for(i=0; i<SAM_ANZAHL; i++)
	{
		strcpy(samname, "sounds\\");
		strcat(samname, samples[i].name);
		fhndl=Fopen(samname, 0);
		if(fhndl<0) return(fhndl);
		flength=Fseek(0L, fhndl, 2);     /* Get file size */
		Fseek(0L, fhndl, 0);

		samadr=(char *)Mxalloc(flength, 0);  /* Speicher reservieren */
		if( ((signed long)samadr)==-32L )  samadr=(char *)Malloc(flength);
		if( ((signed long)samadr)<=0L )  return(samadr);
		Fread(fhndl, flength, samadr);
		Fclose(fhndl);
		samples[i].start=samadr;
		samples[i].end=samadr+flength-2;
	}

	return(0);
}


/* *** De-init sound system (not needed on the atari) *** */
void sound_deinit(void)
{
}


/* ***Neues Sample in Abspielliste aufnehmen oder direkt spielen*** */
void sound_play(short snr)
{
	int i=0;

	if( sndpsgflag && samples[snr].dosnd )
		Dosound(samples[snr].dosnd);
	else if( samples[snr].start )        /* Ignore invalid samples */
	{
		if( ptyp==1 && playflag && tspi )
		{
			tspi->samplay(samples[snr].start, samples[snr].end, aktchan);
			aktchan+=1;
			if(aktchan==4) aktchan=0;
		}
		else if( /*sndsamflag &&*/ !Buffoper(-1) && Locksnd()==1 )
		{
			Devconnect(0, 8, 0, 3, 1);     /* Set 25kHz, for 12.5kHz: use 7 instead of 3 */
			Setmode(2);
			Setbuffer(0, samples[snr].start, samples[snr].end);
			Buffoper(1);                   /* Sample direkt spielen */
			Unlocksnd();
		}
	}

	return;
}
