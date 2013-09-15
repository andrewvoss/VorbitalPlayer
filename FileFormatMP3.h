#ifndef _FILEFORMATMP3_H_
#define _FILEFORMATMP3_H_

#include "AudioFileFormat.h"
#include "mpg123.h"

#include <QString>

class FileFormatMP3: public AudioFileFormat
{
public:
	virtual bool CheckExtension(QString& filename);
	virtual bool Open(QString& filename);
	virtual int GetFormat();
	virtual int GetBitrate();
	virtual int GetChannels();
	virtual int GetSampleRate();
	virtual int FillBuffer(unsigned char* buffer, int numBytes);
	virtual const char* GetComment() { return NULL; }
	static bool Init();
private:
	static mpg123_handle* _mpg123;
	int _channels;
	int _encoding;
	long _sampleRate;
    long _bitRate;
	int _filePosition;
};

#endif
