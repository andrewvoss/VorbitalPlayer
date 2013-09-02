#include "FileFormatMP3.h"
#include <stdio.h>
#include <QMessageBox>

mpg123_handle* FileFormatMP3::_mpg123;

bool FileFormatMP3::CheckExtension(QString& filename)
{
	return filename.contains(".MP3", Qt::CaseInsensitive);
}

bool FileFormatMP3::Open(QString& filename)
{
	_filePosition = 0;
	if( mpg123_open(_mpg123, filename.toStdString().c_str()) != MPG123_OK ||
		mpg123_getformat(_mpg123, &_rate, &_channels, &_encoding) != MPG123_OK )
	{
		QMessageBox(QMessageBox::Critical, filename, QString("Could not open file."), QMessageBox::Ok);
		return false;
	}
    if(_encoding != MPG123_ENC_SIGNED_16 && _encoding != MPG123_ENC_FLOAT_32)
    {
        //cleanup(_mpg123);
        fprintf(stderr, "Bad MP3 encoding for file %s: 0x%x!\n", filename.toStdString().c_str(), _encoding);
        return false;
    }
	// TODO: Figure out if this is necessary.
    /* Ensure that this output format will not change (it could, when we allow it). */
    //mpg123_format_none(mh);
    //mpg123_format(mh, rate, channels, encoding);

	return true;
}

int FileFormatMP3::GetBitrate()
{
	return _rate;
}

int FileFormatMP3::GetChannels()
{
	return _channels;
}

int FileFormatMP3::GetFormat()
{
	return FORMAT_MP3;
}

int FileFormatMP3::GetSampleRate()
{
	return _rate;
}

bool FileFormatMP3::Init()
{
	int err = MPG123_OK;
	err = mpg123_init();
	if( err != MPG123_OK || (_mpg123 = mpg123_new(NULL, &err)) == NULL)
	{
		QMessageBox(QMessageBox::Critical, QString("Unable to initialize MP3 playback library."), QString("ERROR"), QMessageBox::Ok );
		return false;
	}
	return true;
}

int FileFormatMP3::FillBuffer(unsigned char* buffer, int numBytes)
{
	size_t numDone = 0;
	int err = MPG123_OK;
	err = mpg123_read(_mpg123, buffer, numBytes, &numDone);
	_filePosition += numDone;
	if( numDone == 0 || err == MPG123_DONE )
	{
		return -1;
	}
	return numDone;
}
