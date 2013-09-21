VorbitalPlayer
==============

A music player for Linux and Windows with MP3, OGG, and WAV support.

The Vorbital Player was originally written using wxWidgets, but has been ported
to Qt due to some multiplatform portability and consistency annoyances with
wxWidgets. It is written in C++ and uses a number of libraries for file format
support (mpg123, libsndfile, libogg, libvorbis, libwavpack, etc.)

Building for Windows will be a nuisance because you will have to set paths to
all of the various audio libraries (the Windows build isn't as well-maintained
as the Linux build, and the Qt version hasn't been released on Windows yet). If
you just want the app, an installable version of Vorbital 4.0 for Windows is
available here:

http://vorbitalplayer.com/

These libraries were used to build version 4.0 on Windows:
----------------------------------------------------------
OpenAL version 6.14.357.24.            
wrap_oal.dll version 2.2.0.5.            
libogg version 1.3.0.            
libvorbis 1.3.3.          
libsndfile 1.0.25.          
wxWidgets 2.9.4.          
wavpack version 4.60.1.            
libmpg123 version 1.14.2.        

To build an installer for Windows, look in the /installer folder for NullSoft
Installer (NSIS) and InnoSetup projects. File paths may have changed, so you may
need to update them to build an installer.

To build on Linux, you will need Qt 5 and the following packages:

libsndfile-dev
libogg-dev
libvorbis-dev
libopenal-dev
libwavpack-dev
libmpg123-dev

Just do the standard "qmake" and then "make" to build.
