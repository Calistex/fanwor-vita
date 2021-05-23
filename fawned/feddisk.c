/* ******* feddisk.c: Routinen zum Laden und Speichern ******* */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "feddefs.h"
#include "fedmain.h"
#include "level.h"




/* **Variablen** */
char fname[128];			/*Dateinamen */

LEVEL_HEADER hd;


unsigned short swap_short(unsigned short D)
{
	return((D<<8)|(D>>8));
}

unsigned long swap_long(unsigned long D)
{
	return((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}


/* *** Level laden *** */
int loadlevel(char *fname)
{
	int fhndl;
	int i,dx,dy;
	LEVEL_HEADER hd;

	fhndl=open(fname, O_RDONLY);
	if (fhndl<0)
	{
		fprintf(stderr,"Konnte Leveldatei nicht oeffnen!\n");
		return -1;
	}

	if (read(fhndl, &hd, sizeof(LEVEL_HEADER)) != sizeof(LEVEL_HEADER))
	{
		fprintf(stderr, "Failed to read header!\n");
		return -1;
	}

	if(hd.hmagic!=(long)0x4641574FL)  /*'FAWO'*/
	{
		hd.hmagic=swap_long(hd.hmagic);
		hd.version=swap_short(hd.version);
		hd.anz_obj=swap_short(hd.anz_obj);
		hd.r_wdth=swap_short(hd.r_wdth);
		hd.r_hght=swap_short(hd.r_hght);
	}

	if(hd.hmagic!=(long)0x4641574FL)
	{
		fprintf(stderr,"Keine Leveldatei!\n");
		close(fhndl);
		return -1;
	}

	r_width=hd.r_wdth;
	r_height=hd.r_hght;

	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( read(fhndl, &sfeld[dx][dy], sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Fehler beim Lesen der Leveldatei!\n");
				close(fhndl);
				return -1;
			}
		}
	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( read(fhndl, &ffeld[dx][dy], sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Fehler beim Lesen der Leveldatei!\n");
				close(fhndl);
				return -1;
			}
		}

	en_anz=hd.anz_obj;

	i=0;
	while( i < en_anz )
	{
		if( read(fhndl, &en[i], sizeof(LEVEL_EINTRAG)) != sizeof(LEVEL_EINTRAG) )
		{
			fprintf(stderr,"Fehler beim Lesen der Leveldatei!\n");
			close(fhndl);
			en_anz=0;
			return -1;
		}
		i+=1;
	}

	close(fhndl);

	rwx=rwy=0;

	return 0;
}




/* *** Level speichern *** */
int savelevel(char *lname)
{
	int fhndl;
	int dx, dy, i;

	hd.hmagic=0x4641574FL;
	hd.version=0x0250;  /* Version 2.50 */
	hd.anz_obj=en_anz;
	hd.r_wdth=r_width;
	hd.r_hght=r_height;


	fhndl=open(lname, O_WRONLY|O_CREAT,0664);
	if(fhndl<0)
	{
		fprintf(stderr,"Couldn't create levelfile!\n");
		return(fhndl);
	}

	if (write(fhndl, &hd, sizeof(hd)) != sizeof(hd))
	{
		fprintf(stderr, "Could not write level file!\n");
		return -1;
	}

	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( write(fhndl, &sfeld[dx][dy], (long)sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Write error!\n");
				return -1;
			}
		}
	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( write(fhndl, &ffeld[dx][dy], (long)sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Write error!\n");
				return -1;
			}
		}

	i = sizeof(LEVEL_EINTRAG) * en_anz;
	if (write(fhndl, en, (long)i) != i)
	{
		fprintf(stderr, "Write error!\n");
		return -1;
	}

	close(fhndl);

	return 0;
}
