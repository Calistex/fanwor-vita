/* ******* Level structure for Fanwor ******* */

#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

typedef struct
{
	unsigned char eintrtyp;
	unsigned char art;
	unsigned char xpos;
	unsigned char ypos;
	unsigned char xl;
	unsigned char yl;
	unsigned char spec1;
	unsigned char spec2;
} LEVEL_EINTRAG;

/*
eintrtyp:
  1=Enemy
  2=Door
  3=People   - Not yet included
  4=Item
*/


/* header for level files: */

typedef struct
{
	uint32_t hmagic;          /* magic constant, must be 'FAWO' */
	uint16_t version;         /* For checking the version - 0x0123 e.g. would be version 1.23 */
	char titel[32];           /* name of the level */
	uint16_t anz_obj;         /* number of the objects (LEVEL_EINTRAGs) in this room (see above) */
	uint16_t r_wdth, r_hght;  /* Width and height of this room */
} LEVEL_HEADER;

/* Level-header + playing-field + anz_obj * LEVEL_EINTRAGs = Level file */

#endif /* LEVEL_H */
