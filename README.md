VorbitalPlayer
==============

A music player for Windows and Linux with MP3, OGG, and WAV support.

The Vorbital Player was originally written using wxWidgets, but has been ported
to Qt due to some multiplatform portability annoyances with wxWidgets. It is
written in C++ and uses a number of libraries for file format support (mpg123,
libsndfile, libogg, libvorbis, libwavpack, etc)

To build an installer, look in the /installer folder for NullSoft Installer
(NSIS) and InnoSetup projects. File paths may have changed, so you may need
to update them to build an installer.

An installable version of Vorbital 4.0 for Windows, the last wxWidgets version,
is available here:

http://vorbitalplayer.com/

There is still work to be done to get Vorbital Player into shape. See the todo.md
file for more information.
