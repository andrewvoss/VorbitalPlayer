#ifndef __MUSICSTREAM_H__
#define __MUSICSTREAM_H__

#include <iostream>
using namespace std;

#include <AL/al.h>
#include "MusicStream.h"
//#include <speex/speex.h>
#include "wavpack/wavpack.h"
#include "WaveFile.h"
#ifdef WIN32
#include "windows.h"
#endif

#include <QString>
#include <QWindow>
#include <QThread>

#include "Constants.h"
#include "AudioFileFormat.h"

class MusicStream : public QThread
{
public:
	~MusicStream();
	MusicStream(QDialog* parent);
        bool Open(QString file);
        int PlayFile(QString file);
		int Play();
		void Stop();
		bool PausePlayback();
        void Release();
        bool Playback();
		bool IsPlaying();
        bool IsPaused();
        bool Playing();
        bool Update();
		void run();
        void SetVolume( float volume );
		ALuint GetOpenALFormatFromFile(WaveFile* file);
		int GetChannels();
		int GetRate();
		int GetBitrate();
		const char* GetComment();
		bool DecodeSpeex(ALuint buffer);
		bool FillBuffer(ALuint buffer);
protected:
        void Empty();
        void Check();
private:
        FILE* _musicFile;
        ALuint _buffers[2];
        ALuint _source;
        ALenum _format;
		int _playState;
		//SpeexBits _bits;
		void *_speexDecoderState; 
		int _speexFrameSize;
		int _fileFormat;
		WavpackContext * _wavpackContext;
		WaveFile _waveFile;
		unsigned int _wavePosition;
		// For testing purposes.
		QDialog* _parent;
		AudioFileFormat* _audioFile;
};

#endif
