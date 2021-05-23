
                   ***********************************
                   *              FANWOR             *
                   *                                 *
                   *          Version 1.15           *
                   * * * * * * * * * * * * * * * * * *
                   *   Written and copyright(c) by   *
                   *           Thomas Huth           *
                   *           1999, 2017            *
                   * * * * * * * * * * * * * * * * * *
                   * WWW:                            *
                   *  http://fanwor.tuxfamily.org/   *
                   * E-Mail:                         *
                   *  huth at tuxfamily.org          *
                   ***********************************


This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Soft-
ware Foundation; either version 2 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


  1) What's this?
 -----------------

"Fanwor" is a game running on Atari ST/STE/TT/Falcon (and compatibles) and
since version 1.11 it also runs on Linux and other Unixes that are supported
by the SDL Library.



  2) The story
 --------------

Once there was peace on the world Fanwor. Princess Gemda ruled wisely, she
used the eight Gems of Power to protect the kingdom. But one day while Gemda
was away on holiday, the monsters came and stole the Gems! Without these Gems
anarchy broke out and the monsters ruled over the country. They hid the
stones in eight different dungeons because they were unable to destroy the
Gems.
Gemda now sent out the best heros of the world to get the stones back so
that everybody can live in peace again. You are now one of these heros! Try
your best to collect the stones and if you succeed, honor and wealth will
be yours!

...well, this is not the best story I've ever heard of, but it should be
enough ;-)


  3) Playing the game
 ---------------------

Playing the game should be quite easy: Just use the cursor keys to move the
hero around the screen. Press space to use the sword and to start the game.
Most enemies will be quite easy to beat but if you advance to places
that are far away from the start, you will also find some monsters that are
very fast or that have many hit points.

As mentioned above, your task is to collect the eight Gems of Power. You
will start somewhere on the surface of Fanwor and you will have to search
for the eight dungeons where the monsters hid the Gems. At the beginning
you have 20 hitpoints, each time you touch a monster you will loose some.
If you lost all - you lost that game! But you can also regain your hitpoints
by either finding a fairy lake or by collecting one of the Gems - if you
do the latter, you will also get 10 HP in addition.


  4) Technical stuff
 --------------------


 a) Atari GEM version:

Fanwor will run happily in any resolution, now also in high and true color
modes. But Fanwor needs corresponding bitmap graphics to the color depth you
are using (in high and true color, the 256 color graphics are used). Graphics 
for 16 colors and 256 colors are already included in this distribution, if
you want to use Fanwor in monochrome or a 4 color mode, you will have to
create your own grafics (just convert them with a good graphic program).
Since version 1.10 of Fanwor, I enlarged the graphics from 16*16 to 32*32
pixels so that the window is no longer so poorly small. But this has also one
disadvantage: Fanwor does not longer run in ST low resolution, you need now at
lease a 640x400 mode or above. I think the people who are playing GEM games
have a TT, Falcon or a graphic card so that it is okay to not supporting ST low
resolution any more.

Fanwor also supports Joysticks and (Jaguar-)Joypads!
Note that you will have to enable joystick or joypad control before you
can use it - just use the controls dialog that can be found in the options
menu.
Note that I can't guarantee that the joystick and joypad control will work
on your system. Enabling the joystick and the mouse at the same time in GEM
is quite tricky and also a little bit "dirty". Concerning the joypad: I don't
have one, so I was hardly able to test it, but thanks to Sage it should work
now.
When you are running other applications at the same time you are playing
Fanwor and you want to interact via keyboard with them, you should
disable the keyboard control in that dialog because Fanwor maximizes the
key repetition rate to get a good keyboard control.

If you own a Milan or Hades or a tuned Falcon/TT, you will probalby notice
that Fanwor runs too fast for you. Then you should enter the speed dialog and
enlarge the game cycle. Switching off the clipping flag will probably fasten
up the game, but you should use it with caution when you want to move the win-
dow outside the screen.

Concerning the sound, Fanwor is able to either play Yamaha-PSG chip music
sounds or digital sample sounds. Just use the sound dialog in the option
menu to switch between them. Both PSG sounds and sample sounds are
played via the XBIOS so that the system should never crash when you enable
one of them although you don't have the right hardware in your computer.
If you have either a new version of TeTra, Paula or Ultimate Tracker installed
as an accessory, you can also automatically listen to a MOD file while you are
playing - but note that Fanwor will automatically switch back to Yamaha-PSG
sound fx if you enable the MOD playing!
On a Falcon030, TT or a accelerated STE there is also the possibility to listen
to the MOD and to use the sample sounds at the same time! Fanwor will try to
use some special sound drivers (the *.SPI files in the music\ directory).
But note that these are experimental things... if you have problems with
them, you should either rename them so that Fanwor can't find them or you
shouldn't enable the MOD playing.
Don't use the DMA.SPI on a 8MHz STE and if you have an accelerated Falcon
you should be prepared for crashes when you use the DSP.SPI!

Fanwor will also runs happily from a floppy disk, but you should really
install it on your hard disk because the levels are not cached and so using a
floppy will be very slow.


 b) Linux/Unix SDL version

You probably might need to recompile Fanwor for your system. For doing this,
you need to have installed a C-compiler (like GCC or clang) and some additional
libraries with their development files first:

- SDL v1.2 (http://libsdl.org/download-1.2.php)
- SDL_mixer v1.2 (https://www.libsdl.org/projects/SDL_mixer/release-1.2.html)
- libpng (http://www.libpng.org/pub/png/)
- zlib (http://www.zlib.net/)

All of these libraries are pretty standard nowadays, so you should also be able
to find the corresponding packages in your distribution.

Once you've got all required development libraries installed correctly, go to
the src directory and type "make". Fanwor's Makefile is not very keen - if
you have problems to compile Fanwor, you must probably edit the Makefile to
suit your system (have especially a look the lines with LIBS_sdl and CFL_sdl).

If you have installed the SDL, the SDL-mixer, the PNG and the ZLib library
correctly and still don't succeed in compiling Fanwor, then please contact me!

There is also one command line switch:
 -f  Try to use a fullscreen video mode.

To quit the game, simply press ESC.


                    And now have fun with Fanwor!
