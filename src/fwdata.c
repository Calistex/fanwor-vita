/*************************************************/
/* fwdata.c  -  sprite table                     */
/* This will be in a external file one day...    */
/* * * * * * * * * * * * * * * * * * * * * * * * */
/* Written and (c) by Thomas Huth                */
/* See fwsource.txt for more information         */
/* * * * * * * * * * * * * * * * * * * * * * * * */
/* This file is distributed under the GNU Public */
/* License, version 2 or at your option any la-  */
/* ter version. Read "gpl.txt" for details.      */
/*************************************************/

#include "fwdefs.h"
#include "fwdata.h"

#define WPST 2
#define ENST 3
#define ITST 15

SPRTTABL spritetable[DIFSPRTNUM];

void setsprttabl(void)
{
	int i;

	spritetable[0].class=0;
	spritetable[0].id=0;
	spritetable[0].movement=0;
	spritetable[0].hp=20;
	for(i=0; i<4; i++)
		spritetable[0].animnr[i]=2;

	spritetable[1].class=0;
	spritetable[1].id=1;
	spritetable[1].hp=0;
	spritetable[1].movement=0;
	spritetable[1].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[1].animnr[i]=0;

	spritetable[WPST+0].class=CWEAPN;
	spritetable[WPST+0].id=0;
	spritetable[WPST+0].hp=2;
	spritetable[WPST+0].movement=0;
	for(i=0; i<4; i++)
		spritetable[WPST+0].animnr[i]=1;

	spritetable[ENST+0].class=CENMY;
	spritetable[ENST+0].id=0;
	spritetable[ENST+0].hp=2;
	spritetable[ENST+0].movement=2;
	for(i=0; i<4; i++)
		spritetable[ENST+0].animnr[i]=2;

	spritetable[ENST+1].class=CENMY;
	spritetable[ENST+1].id=1;
	spritetable[ENST+1].hp=3;
	spritetable[ENST+1].movement=2;
	for(i=0; i<4; i++)
		spritetable[ENST+1].animnr[i]=2;

	spritetable[ENST+2].class=CENMY;
	spritetable[ENST+2].id=2;
	spritetable[ENST+2].hp=2;
	spritetable[ENST+2].movement=3;
	spritetable[ENST+2].animnr[0]=2;
	for(i=1; i<4; i++)
		spritetable[ENST+2].animnr[i]=0;

	spritetable[ENST+3].class=CENMY;
	spritetable[ENST+3].id=3;
	spritetable[ENST+3].hp=2;
	spritetable[ENST+3].movement=1;
	spritetable[ENST+3].animnr[0]=2;
	for(i=1; i<4; i++)
		spritetable[ENST+3].animnr[i]=0;

	spritetable[ENST+4].class=CENMY;
	spritetable[ENST+4].id=4;
	spritetable[ENST+4].hp=4;
	spritetable[ENST+4].movement=1;
	spritetable[ENST+4].animnr[0]=2;
	for(i=1; i<4; i++)
		spritetable[ENST+4].animnr[i]=0;

	spritetable[ENST+5].class=CENMY;
	spritetable[ENST+5].id=5;
	spritetable[ENST+5].hp=2;
	spritetable[ENST+5].movement=4;
	spritetable[ENST+5].animnr[0]=2;
	for(i=1; i<4; i++)
		spritetable[ENST+4].animnr[i]=0;

	spritetable[ENST+6].class=CENMY;
	spritetable[ENST+6].id=6;
	spritetable[ENST+6].hp=3;
	spritetable[ENST+6].movement=4;
	spritetable[ENST+6].animnr[0]=2;
	for(i=1; i<4; i++)
		spritetable[ENST+6].animnr[i]=0;

	spritetable[ENST+7].class=CENMY;
	spritetable[ENST+7].id=7;
	spritetable[ENST+7].hp=3;
	spritetable[ENST+7].movement=2;
	for(i=0; i<4; i++)
		spritetable[ENST+7].animnr[i]=2;

	spritetable[ENST+8].class=CENMY;
	spritetable[ENST+8].id=8;
	spritetable[ENST+8].hp=5;
	spritetable[ENST+8].movement=2;
	for(i=0; i<4; i++)
		spritetable[ENST+8].animnr[i]=2;

	spritetable[ENST+9].class=CENMY;
	spritetable[ENST+9].id=9;
	spritetable[ENST+9].hp=5;
	spritetable[ENST+9].movement=2;
	for(i=0; i<4; i++)
		spritetable[ENST+9].animnr[i]=2;

	spritetable[ENST+10].class=CENMY;
	spritetable[ENST+10].id=10;
	spritetable[ENST+10].hp=7;
	spritetable[ENST+10].movement=2;
	for(i=0; i<4; i++)
		spritetable[ENST+10].animnr[i]=2;

	spritetable[ENST+11].class=CENMY;
	spritetable[ENST+11].id=11;
	spritetable[ENST+11].hp=9;
	spritetable[ENST+11].movement=1;
	for(i=0; i<4; i++)
		spritetable[ENST+11].animnr[i]=1;


	spritetable[ITST+0].class=CITEM;
	spritetable[ITST+0].id=0;
	spritetable[ITST+0].hp=0;
	spritetable[ITST+0].movement=0;
	spritetable[ITST+0].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+0].animnr[i]=0;

	spritetable[ITST+1].class=CITEM;
	spritetable[ITST+1].id=1;
	spritetable[ITST+1].hp=0;
	spritetable[ITST+1].movement=0;
	spritetable[ITST+1].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+1].animnr[i]=0;

	spritetable[ITST+2].class=CITEM;
	spritetable[ITST+2].id=2;
	spritetable[ITST+2].hp=0;
	spritetable[ITST+2].movement=0;
	spritetable[ITST+2].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+2].animnr[i]=0;

	spritetable[ITST+3].class=CITEM;
	spritetable[ITST+3].id=3;
	spritetable[ITST+3].hp=0;
	spritetable[ITST+3].movement=0;
	spritetable[ITST+3].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+3].animnr[i]=0;

	spritetable[ITST+4].class=CITEM;
	spritetable[ITST+4].id=4;
	spritetable[ITST+4].hp=0;
	spritetable[ITST+4].movement=0;
	spritetable[ITST+4].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+4].animnr[i]=0;

	spritetable[ITST+5].class=CITEM;
	spritetable[ITST+5].id=5;
	spritetable[ITST+5].hp=0;
	spritetable[ITST+5].movement=0;
	spritetable[ITST+5].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+5].animnr[i]=0;

	spritetable[ITST+6].class=CITEM;
	spritetable[ITST+6].id=6;
	spritetable[ITST+6].hp=0;
	spritetable[ITST+6].movement=0;
	spritetable[ITST+6].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+6].animnr[i]=0;

	spritetable[ITST+7].class=CITEM;
	spritetable[ITST+7].id=7;
	spritetable[ITST+7].hp=0;
	spritetable[ITST+7].movement=0;
	spritetable[ITST+7].animnr[0]=1;
	for(i=1; i<4; i++)
		spritetable[ITST+7].animnr[i]=0;


	spritetable[ENST+20].class=CENMY;
	spritetable[ENST+20].id=12;
	spritetable[ENST+20].hp=1;
	spritetable[ENST+20].movement=3;
	spritetable[ENST+20].animnr[0]=2;
	for(i=1; i<4; i++)
		spritetable[ENST+20].animnr[i]=0;

	spritetable[ENST+21].class=CENMY;
	spritetable[ENST+21].id=13;
	spritetable[ENST+21].hp=4;
	spritetable[ENST+21].movement=2;
	spritetable[ENST+21].animnr[0]=2;
	for(i=1; i<4; i++)
		spritetable[ENST+21].animnr[i]=0;

	spritetable[ENST+22].class=CENMY;
	spritetable[ENST+22].id=14;
	spritetable[ENST+22].hp=6;
	spritetable[ENST+22].movement=2;
	for(i=0; i<4; i++)
		spritetable[ENST+22].animnr[i]=2;

	spritetable[ENST+23].class=CENMY;
	spritetable[ENST+23].id=15;
	spritetable[ENST+23].hp=2;
	spritetable[ENST+23].movement=5;
	for(i=0; i<4; i++)
		spritetable[ENST+23].animnr[i]=2;

}
