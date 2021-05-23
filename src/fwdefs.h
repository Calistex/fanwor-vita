/*********************************************/
/* fwdefs.h  -  General definitions          */
/* * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth            */
/* See fwsource.txt for more information     */
/*********************************************/

#ifndef _FWDEFS_H
#define _FWDEFS_H

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


#define MAXSPRTNR   256     /* Maximal sprite number */
#define MAXDOORNR   16      /* Max. door number */
#define DIFSPRTNUM  27      /* Number of different sprite types */

#define FWALL  1
#define FDOOR  2
#define FWATER 4

#define CPEOPL 1
#define CITEM  2
#define CWEAPN 3
#define CENMY  4

typedef struct              /* Sprite type */
{
	short class;
	short index;
	unsigned short grfyoffset[4];
	short x, y;
	short oldx, oldy;
	short xpos, ypos;
	short oldxpos, oldypos;
	short hp;
	short directn;
	short movement;
	short anim;
	short step;
	short counter;
	short maxcnter;
	short spec;
} SPRT;

typedef struct
{
	short class;
	short id;
	short hp;
	short movement;
	short animnr[4];
} SPRTTABL;

typedef struct              /* Door structure */
{
	unsigned char x, y;
	unsigned char destnr;
	unsigned char destx, desty;
	unsigned char exitx, exity;
} DOOR;

#endif /* _FWDEFS_H */
