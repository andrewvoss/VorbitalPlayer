Still Needed for Qt Port:
-------------------------
- Verify that album art loading works
- Fix playlist save/load. Saving seems to work, but loading doesn't.
- Allow dialog resizing after load (set min size vs set fixed size).
- Fix control layout and button positioning/size.
- Make right-click on playlist for file info work.

NOTE: This section may no longer be accurate with the port to Qt for the interface.
Re-check everything mentioned in this file.

----------------------- FUTURE TODO -------------------
*21)Fix pause when playing a track after playlist has been cleared.
*24)Add FLAC support.  Right now it's partial but the library sucks and doesn't work.
*27)Add wrench icon that lets users set interface colors.
*28)Compare interface to Winamp, WMP, others to see what can be done about improvements.
*29)Test app with Windows 7, Windows Vista, and Windows XP.
*34)Add skinning to let user load an image file and use it as dialog background.  Remember setting.
*35)Add settings save and load to remember skin, volume, playlist contents, color scheme, etc.
*36)M3U playlist support (both load and save).
*37)Show more details on item right-click in playlist.
*38)Add ID3 tag editing.

BUGS:

Organ1 (32-bit sample) does not play right in these formats:
ogg (distorted)
wav (distorted)
    - GetOpenALFormatFromFile assigns 16 bits to 32-bit wav files.
wavpack (extremely distorted)

A0-1.wv (8-bit mono) does not play right in these formats:
wavpack (no sound is heard)

Fantasynth (16-bit mono):
Plays fine in all formats: AIF, OGG, SND, WAV, WV

Apocalyptica-Seemann (16-bit stereo):
Plays fine in all formats: AIF, SND, WAV, WV (no OGG file present)

Untested:
32-bit aiff/aif/snd.  Have not verified that those formats support 32-bit.

LIBRARY NOTES FOR 4.0
Current OpenAL version 6.14.357.24.  No change.
Current wrap_oal.dll version 2.2.0.5.  No change.
Current libogg version 1.3.0.  No change.
Current libvorbis 1.3.3  No change
Current libsndfile 1.0.25  No change
Current wxWidgets 2.9.4  No change
Current wavpack version 4.60.1.  No change.
Current libmpg123 version 1.14.2.  Newly added.

LIBRARY NOTES FOR 3.11
Current OpenAL version is  6.14.357.24.  No change.
Current wrap_oal.dll version is  2.2.0.5.  No change.
*Updated libogg version updated from 1.2.2 --> 1.3.0
*Updated libvorbis from 1.3.2 --> 1.3.3 (short file playback fix)
Updated libsndfile from 1.0.24 --> 1.0.25 (security fix)
Updated wxWidgets from 2.8.11 to 2.9.4
Current wavpack version 4.60.1.  No change.

LIBRARY NOTES FOR 3.1
Current OpenAL version is  6.14.357.24.  No change.
Current wrap_oal.dll version is  2.2.0.5.  No change.
Current Ogg version updated 1.2.1 --> 1.2.2
Current Vorbis version 1.3.2.  No change.
Current libsndfile version updated 1.0.21 --> 1.0.24
Current wxWigets version is 2.8.11.  No change.
Current wavpack version 4.60.1.  No change.

LIBRARY NOTES FOR 3.0:
Previous OpenAL version was 6.14.357.11
Current  OpenAL version is  6.14.357.24
Previous wrap_oal.dll version was 2.0.0.0
Current  wrap_oal.dll version is  2.2.0.5
Current  Ogg    version 1.1.4 --> 1.2.1
Current  Vorbis version 1.2.3 --> 1.3.2
Current  Wavpack version 4.60.1
Current  libsndfile version 1.0.21
Current  libspeex version 1.2rc1
Current  wxWidgets version 2.8.10 --> 2.8.11
