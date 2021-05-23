# ***Makefile for Fanwor***

# Select your architecture:
# - sdl   : Linux/Unix SDL Library
# - atari : Atari GEM
ARCH=sdl

# If you don't need/want music on Linux with the SDL-Mixer lib,
# you can comment out the following two lines:
SOUNDLIB = -lSDL_mixer
SOUNDCFL = -DWITH_SOUND

# What io library do we need? Perhaps you also have to edit this:
LIBS_sdl = -lpng $(shell sdl-config --libs) -lz $(SOUNDLIB)
LIBS_atari = -laes -lvdi

# System specific CFLAGS:
CFL_sdl = $(shell sdl-config --cflags) $(SOUNDCFL) -g -O2 \
	  -Wall -Wmissing-prototypes -Wstrict-prototypes
CFL_atari = 


ARCHDIR=./arch/$(ARCH)

CFLAGS=-I. -I./arch/$(ARCH) $(CFL_$(ARCH))

LIBS = $(LIBS_$(ARCH))


OBS_MAIN = fwdisk.o fwfight.o fwreact.o fwmain.o fwdata.o
OBS_ARCH = $(ARCHDIR)/fwguiini.o $(ARCHDIR)/fwgui.o $(ARCHDIR)/fwgraf.o $(ARCHDIR)/fwmusic.o
OBS_atari = ./arch/atari/loadimg.o ./arch/atari/windial.o ./arch/atari/fw_asm.o
OBS_sdl = ./arch/sdl/loadpng.o
OBS_XTRA = $(OBS_$(ARCH))


fanwor: $(OBS_MAIN) $(OBS_ARCH) $(OBS_XTRA)
	$(CC) $(OBS_MAIN) $(OBS_ARCH) $(OBS_XTRA) $(LIBS) -o../fanwor



fwmain.o: fwdefs.h
fwreact.o: fwdefs.h
fwdisc.o: level.h


clean:
	$(RM) $(OBS_MAIN) $(OBS_ARCH) $(OBS_XTRA) *.bak *~
