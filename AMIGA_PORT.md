OpenMRac for GFOS
=================

A port of OpenMRac, Franticware's split-screen 3D car racing game (the open
source release of MultiRacer), from https://github.com/Franticware/OpenMRac
at commit c301961d28eda40c2d30245160d87964b9cccd99 (OpenMRac 1.3), with its
data from https://github.com/Franticware/OpenMRac-data at commit
e68c2bb7636d08668ed11c5862c923d3db41c0c2, for GFOS: AmigaOS 3.2 with
GFOpenUp, on a 68040 or 68060 with an FPU, on GFOpenUp's SDL 2 and OpenGL.

Race three cars (in four colours each) around four tracks (a speedway,
a boulevard, an ironworks and a suburb), forwards or backwards, by day or
in the evening, against your own best lap's ghost or with up to four
players on one split screen, on the keyboard or joysticks.

Status: host tests pass (played through the keyboard on a virtual X
display, on the host's OpenGL); not yet built in the stove or run in a lab.


What you need
-------------

- GFOS: AmigaOS 3.2 with GFOpenUp 0.8.26 or later, which installs
  LIBS:GFOpenGPU/SDL2.module, SDL2_image.module and GL.module. On
  AmiCradle GL is drawn by the PC's GPU (virgl); on other Amigas by Mesa's
  softpipe on the CPU, which is slow on a 68040.
- A 68040 or 68060 with an FPU (not an LC040 or EC040), an RTG screen with
  room for a 640 x 480 window, and about 32 MB of free memory.
- Nothing else: the game data are in the drawer.


How to play
-----------

Install it one of three ways:

- GFOpenUp: `GFOpenUpTool INSTALL Parts/OpenMRac` puts it in
  SYS:Games/OpenMRac.
- Run Install-OpenMRac from the archive (it offers Work:Games).
- Or copy the OpenMRac drawer where you want it.

Then double-click OpenMRac. A settings window comes first (the renderer,
the window size, vsync, the texture filter and the sound rate); OK starts
the game. On GFOS it starts at 640 x 480, bilinear filtering, 22050 Hz
sound and one player; the settings window and the game's menus change all
of them. From a Shell, `OpenMRac --skip-settings` goes straight to the
game.

The menus are played with the cursor keys and Return, Esc goes back.
Player one drives with the cursor keys (Up accelerates, Down brakes);
Controls in the menu sets the keys or joysticks of all four players.

The settings (settings-sdl2.dat) and the best laps' ghosts are kept in the
drawer's User drawer. Install-OpenMRac and GFOpenUp keep User when they
update the game.


Building
--------

    sh package/build-amiga.sh        (STOVE=/path/to/os32-gcc16-open6)

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target
package. It fetches upstream and its data at the commits
package/openmrac.lock names, applies amiga/patches, makes
Data/openmrac.dat (tools/make_dat.py), builds with amiga/Makefile.amigaos
and packs OpenMRac-VERSION-68040.lha and the GFOpenUp part Parts/OpenMRac.

Host tests: `sh tests/host/test_all.sh` (Linux, gcc, SDL 2, SDL2_image and
GL development files; GLM, libpng and libjpeg too for the comparisons;
Xvfb for the played test) checks that the patch applies; that
openmrac.dat holds every data file unchanged and every sound as its WAVE
file's samples; builds the whole game with the host's compiler through the
same Makefile, its GLM answered by our stand-in only; runs every GLM call
the game makes on our stand-in and on GLM itself, and requires the same
results to the last bit; reads every picture of the data through our
SDL_image reader and through upstream's libpng and libjpeg readers, and
requires the same pixels; tests the User drawer; and plays the game on a
virtual X display through the keyboard, from the settings window (which
must show the Amiga defaults and save them in PROGDIR:User) through the
menu to a race, where the car must move.


What was changed
----------------

- amiga/compat/glm: OpenMRac does its matrix sums with GLM, which is not
  part of GFOS; this is our own small stand-in for exactly the calls it
  makes, with GLM's results to the last bit (see Host tests).
- amiga/compat/pict2_sdlimage.cpp: the textures are read through
  GFOpenGPU's SDL2_image (which hands JPEG and PNG to the GPU's cores where
  it can) instead of a libjpeg and a libpng built into the game. It
  replaces upstream's pict2_jpeg.cpp and pict2_png.cpp.
- The sound is upstream's own mixer, MiniAL, on SDL 2's audio, not OpenAL
  (upstream's USE_MINIAL).
- amiga/patches/0001-amigaos-user-drawer-defaults.patch:
  - the settings and ghosts are kept in PROGDIR:User/ (not SDL's
    preferences drawer);
  - the first start's defaults for a 68040: 640 x 480, bilinear
    filtering, 22050 Hz, one player.
- The data are read from PROGDIR:Data/openmrac.dat (DIR_OPENMRAC_DAT, set
  by the Makefile).
- amiga/amiga_start.c: 256 KB of stack, the $VER string, and PROGDIR:User
  made at start.
- amiga/: the Makefile, the GlowIcons (our own art), the Installer script
  and its spec. package/ and tools/: the build, the data file, the release
  archive and the GFOpenUp part.

The AmiCradle Team.
