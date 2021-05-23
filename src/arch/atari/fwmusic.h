/* **** Prototypen & defines for fwmusic.c **** */

#ifndef _FWMUSIC_H
#define _FWMUSIC_H

#define S_SWORD         0
#define S_TAKEITEM      1
#define S_SUCCESS       2

#if 0
typedef struct
{
	char *name;
	void *start;
	void *end;
	void *dosnd;
} THSAMPLE;

extern THSAMPLE samples[];
#endif

extern short sndpsgflag;
extern short sndsamflag;
extern short sndmodflag;
extern int ptyp;
extern short playflag;

int mod_init(void);
int mod_play(const char *mname);
int mod_stop(void);
long sound_init(void);
void sound_deinit(void);
void sound_play(short snr);

#endif /* _FWMUSIC_H */
