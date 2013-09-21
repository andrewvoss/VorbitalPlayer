#ifndef _AUDIOFILEFORMAT_H_
#define _AUDIOFILEFORMAT_H_

#include "Constants.h"

#include <QString>

class AudioFileFormat
{
public:
	AudioFileFormat() {}
	virtual ~AudioFileFormat() {}
	// For checking whether a file has the right extension for this format.
	virtual bool CheckExtension(QString& filename) = 0;
	// Open a file for reading and get its properties (bit rate, sample rate, channels, etc.)
	virtual bool Open(QString& filename) = 0;
	// Gets the file format enum.
	virtual int GetFormat() = 0;
	// Gets the bitrate of the file.
	virtual int GetBitrate() = 0;
	// Gets the number of channels for the file.
	virtual int GetChannels() = 0;
	// Gets the sample rate for the file.
	virtual int GetSampleRate() = 0;
	virtual int FillBuffer(unsigned char* buffer, int numBytes) = 0;
	static bool Init();
};

#endif
