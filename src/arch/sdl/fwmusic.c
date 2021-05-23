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

#define WITH_SOUND

#ifdef WITH_SOUND

#include <SDL/SDL_mixer.h>
#include "fwmusic.h"

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define SAM_ANZAHL 3


short playflag=0;

static Mix_Music *tmusic = NULL;
static Mix_Chunk *samples[SAM_ANZAHL];
static char *samnames[SAM_ANZAHL] =
{
	"sword.wav", "takeitem.wav", "success.wav"
};



/* ############# Funktionen ############### */


/* ***Init the MOD player (not needed for the SDL Mixer)*** */
int mod_init(void)
{
	return 0;
}


/* ***Play a MOD*** */
int mod_play(const char *mname)
{
	if (playflag)
	{
		Mix_HaltMusic();
		playflag=FALSE;
	}

	/* Load the requested music file */
	tmusic = Mix_LoadMUS(mname);
	if ( tmusic == NULL )
	{
		fprintf(stderr, "Couldn't load the mod: %s\n", SDL_GetError());
		return 1;
	}

	Mix_PlayMusic(tmusic, -1);

	playflag=TRUE;

	return 0;
}

/* ***Stop MOD playing*** */
int mod_stop(void)
{
	Mix_HaltMusic();
	playflag=FALSE;
	return 0;
}



/* ***Init the sound subsystem, load samples*** */
long sound_init(void)
{
	int audio_rate;
	Uint16 audio_format;
	int audio_channels;
	int audio_buffers;
	char aktsamname[32];
	int i;

	/* Initialize variables */
	audio_rate = 44100; // 22050;
	audio_format = AUDIO_S16; //AUDIO_S8;
	audio_channels = 2;
	audio_buffers = 1024;

	/* Open the audio device */
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0)
	{
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		return 1;
	}
	else
	{
		Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
		/*printf("Opened audio at %d Hz %d bit %s, %d bytes audio buffer\n", audio_rate,
		         (audio_format&0xFF),
		         (audio_channels > 1) ? "stereo" : "mono",
		         audio_buffers);*/
	}


	/* Load the sample music files */
	for (i = 0; i < SAM_ANZAHL; i++)
	{
		strcpy(aktsamname, "app0:sounds/");
		strcat(aktsamname, samnames[i]);
		samples[i] = Mix_LoadWAV(aktsamname);
		if ( samples[i] == NULL )
		{
			fprintf(stderr, "Couldn't load %s: %s\n", aktsamname, SDL_GetError());
		}
	}

	return 0;
}


/* ***Deinit the sound system*** */
void sound_deinit(void)
{
	int i;
	if ( tmusic )
	{
		Mix_FreeMusic(tmusic);
		tmusic = NULL;
	}

	for (i = 0; i < SAM_ANZAHL; i++)
		if(samples[i])
			Mix_FreeChunk(samples[i]);

	Mix_CloseAudio();
}



/* ***Play a sample sound*** */
void sound_play(short snr)
{
	if(samples[snr])
	{
		Mix_PlayChannel(-1,samples[snr], 0);
	}
}



#else  /* WITH_MUSIC */



int mod_init(void)
{
	return 0;
}

int mod_play(const char *mname)
{
	return 0;
}

int mod_stop(void)
{
	return 0;
}

long sound_init(void)
{
	return 0;
}

void sound_deinit(void)
{
}

void sound_play(short snr)
{
}


#endif /* WITH_MUSIC */
