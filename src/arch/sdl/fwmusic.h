/* **** Prototypes and defines for fwmusic.c **** */

#ifndef _FWMUSIC_H
#define _FWMUSIC_H

#define S_SWORD         0
#define S_TAKEITEM      1
#define S_SUCCESS       2

extern short playflag;

int mod_init(void);
int mod_play(const char *mname);
int mod_stop(void);
long sound_init(void);
void sound_play(short snr);
void sound_deinit(void);

#endif /* _FWMUSIC_H */
