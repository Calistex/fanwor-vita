#Makefile for "FAWNED - The FANWOR EDITOR"

OBS=fedinit.o fedgraf.o fedkeys.o feddisk.o fedmain.o \
    sdlgui.o dlgAlert.o dlgFileSelect.o file.o
FWOBS=../src/fwdata.o ../src/arch/sdl/loadpng.o

CFLAGS = -g -O1 -Wall $(shell sdl-config --cflags)
CPPFLAGS = -I../src -I../src/arch/sdl
LIBS = -lpng $(shell sdl-config --libs)

fawned: $(OBS) $(FWOBS)
	$(CC) $(OBS) $(FWOBS) $(LIBS) -o fawned

fedinit.o: feddefs.h fedmain.h fedgraf.h

fedgraf.o: fedgraf.c feddefs.h fedmain.h ../src/fwdata.c ../src/fwdefs.h

fedkeys.o: fedmain.h

fedmain.o: fedmain.c feddefs.h fedinit.h fedgraf.h ../src/level.h ../src/fwdefs.h

feddisk.o: ../src/level.h

$(FWOBS):
	$(MAKE) -C ../src

clean:
	$(RM) fawned *.o *.bak
