#include "MusicStream.h"
#include "FileFormatMP3.h"
#include "FileFormatVorbis.h"

#include <QMessageBox>

MusicStream::MusicStream(QDialog* parent)
{
	_parent = parent;
	//speex_bits_init(&_bits);
	//_speexDecoderState = speex_lib_get_mode(SPEEX_GET_HIGH_MODE);
	//speex_decoder_ctl(_speexDecoderState, SPEEX_GET_FRAME_SIZE, &_speexFrameSize);
    _musicFile = NULL;
	_source = 0;
	_buffers[0] = 0;
	_buffers[1] = 0;
	_format = 0;
    printf("Setting MusicStream play state to STOPPED.\n");
	_playState = STOPPED;
	FileFormatVorbis::Init();
	FileFormatMP3::Init();
}

MusicStream::~MusicStream()
{
	//speex_bits_destroy(&_bits);
	//speex_decoder_destroy(_speexDecoderState);
	Release();
}

bool MusicStream::Open(QString file)
{
    printf("Loading file: %s\n", file.toStdString().c_str());
    //int result = 0;

	//else if( file.Contains(".spx"))
/*	{
		_fileFormat = FORMAT_SPEEX;
	}
	else*/
	if( file.contains(".mp3", Qt::CaseInsensitive) )
	{
		_audioFile = new FileFormatMP3();
		bool opened = _audioFile->Open(file);
        if( !opened ) return false;
		_fileFormat = _audioFile->GetFormat();
		if( _audioFile->GetChannels() == 1 )
		{
			_format = AL_FORMAT_MONO16;
		}
		else
		{
			_format = AL_FORMAT_STEREO16;
		}
	}
	else if( file.contains(".wav", Qt::CaseInsensitive) )
	{
		_wavePosition = 0;
		_fileFormat = FORMAT_WAVE;
		_waveFile.Load(file.toStdString().c_str());
		_format = GetOpenALFormatFromFile(&_waveFile);
	}
	else if( file.contains(".snd", Qt::CaseInsensitive) )
	{
		_wavePosition = 0;
		_fileFormat = FORMAT_WAVE;
		_waveFile.Load(file.toStdString().c_str());
		_format = GetOpenALFormatFromFile(&_waveFile);
	}
	//else if( file.contains(".flac", Qt::CaseInsensitive) )
	//{
	//	_wavePosition = 0;
	//	_fileFormat = FORMAT_WAVE;
	//	_waveFile.Load(file.mb_str());
	//	_format = GetOpenALFormatFromFile(&_waveFile);
	//}
	else if( file.contains(".aif", Qt::CaseInsensitive) || file.contains(".aiff", Qt::CaseInsensitive) )
	{
		_wavePosition = 0;
		_fileFormat = FORMAT_WAVE;
		_waveFile.Load(file.toStdString().c_str());
		_format = GetOpenALFormatFromFile(&_waveFile);
	}
	else if( file.contains(".wv", Qt::CaseInsensitive) )
	{
		_fileFormat = FORMAT_WAVPACK;
		char* error = NULL;
		_wavpackContext = WavpackOpenFileInput (file.toStdString().c_str(), error, OPEN_WVC|OPEN_2CH_MAX|OPEN_NORMALIZE|OPEN_STREAMING, 1);
		if( GetChannels() == 1 )
		{
			_format = AL_FORMAT_MONO16;
		}
		else
		{
			_format = AL_FORMAT_STEREO16;
		}
	}
	else if( file.contains(".ogg", Qt::CaseInsensitive) )
	{
		_audioFile = new FileFormatVorbis();
		bool opened = _audioFile->Open(file);
        if( !opened ) return false;
		_fileFormat = _audioFile->GetFormat();
		if( _audioFile->GetChannels() == 1 )
		{
			_format = AL_FORMAT_MONO16;
		}
		else
		{
			_format = AL_FORMAT_STEREO16;
		}
	}

	alGenBuffers(2, _buffers);
    Check();
	alGenSources(1, &_source);
    Check();

    alSource3f(_source, AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(_source, AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(_source, AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (_source, AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (_source, AL_SOURCE_RELATIVE, AL_TRUE      );

	return true;
}

void MusicStream::Release()
{
    alSourceStop(_source);
    Empty();
    alDeleteSources(1, &_source);
    Check();
    alDeleteBuffers(2, _buffers);
    Check();
	if( _audioFile != NULL )
	{
		delete _audioFile;
	}
}

/**
* Gets the number of channels in the currently playing file.
*/
int MusicStream::GetChannels()
{
	if( _fileFormat == FORMAT_SPEEX )
	{
		return 1;
	}
	else if( _fileFormat == FORMAT_VORBIS || _fileFormat == FORMAT_MP3 )
	{
		return _audioFile->GetChannels();
	}
	else if( _fileFormat == FORMAT_WAVPACK )
	{
		return WavpackGetReducedChannels(_wavpackContext);
	}
	else if( _fileFormat == FORMAT_WAVE )
	{
		return _waveFile.GetNumChannels();
	}
	return 2;
}

/**
* Gets the sample rate for the currently playing file.
*/
int MusicStream::GetRate()
{
	if( _fileFormat == FORMAT_SPEEX )
	{
		return 1;
	}
	else if( _fileFormat == FORMAT_WAVPACK )
	{
		return WavpackGetSampleRate(_wavpackContext);
	}
	else if( _fileFormat == FORMAT_VORBIS )
	{
		return _audioFile->GetSampleRate();
	}
	else if( _fileFormat == FORMAT_WAVE )
	{
		return _waveFile.GetSampleRate();
	}
	else if( _fileFormat == FORMAT_MP3 )
	{
		if( _audioFile != NULL )
		{
			return _audioFile->GetSampleRate();
		}
	}
	return 0;
}

/**
* Gets the bitrate for the currently playing file.  For most formats, this is the sample
* rate times the block alignment.
*/
int MusicStream::GetBitrate()
{
	if( _fileFormat == FORMAT_SPEEX )
	{
		return 1;
	}
	else if( _fileFormat == FORMAT_VORBIS || _fileFormat == FORMAT_MP3 )
	{
		return _audioFile->GetBitrate();
	}
	else if( _fileFormat == FORMAT_WAVPACK )
	{
		return WavpackGetReducedChannels(_wavpackContext) * WavpackGetSampleRate(_wavpackContext) * (WavpackGetBitsPerSample(_wavpackContext) ) * 8;
	}
	else if( _fileFormat == FORMAT_WAVE )
	{
		return _waveFile.GetSampleRate() * _waveFile.GetBlockAlign() * 8;
	}
	return 0;
}

// TODO: Refactor this so that playback and update both use a common code block to
// buffer a block of data.
bool MusicStream::Playback()
{
    if( _playState == PAUSED )
    {
        return false;
    }

    if(Playing() )
    {
        return true;
    }

	if( _fileFormat == FORMAT_SPEEX )
	{
		if( !DecodeSpeex(_buffers[0]))
			return false;

		if( !DecodeSpeex(_buffers[1]))
			return false;
		alSourceQueueBuffers(_source, 2, _buffers);
	}
	else if( _fileFormat == FORMAT_VORBIS )
	{
		bool result = FillBuffer(_buffers[0]);
		if( !result )
		{
			return false;
		}
		result = FillBuffer(_buffers[1]);
		if( !result )
		{
			return false;
		}
		alSourceQueueBuffers(_source, 2, _buffers );
	}
	else if( _fileFormat == FORMAT_WAVE )
	{
		unsigned int totalSamples = _waveFile.GetNumSamples();
		int avail = totalSamples - _wavePosition;
		if( avail > WAVE_CHUNK_SIZE )
		{
			avail = WAVE_CHUNK_SIZE;
		}
		short* chunk1 = _waveFile.GetChunk(_wavePosition,_wavePosition + avail);
		alBufferData(_buffers[0], _format, chunk1, avail*_waveFile.GetNumChannels()*2, _waveFile.GetSampleRate());
		delete[] chunk1;
		_wavePosition += avail;
		avail = totalSamples - _wavePosition;
		if( avail > WAVE_CHUNK_SIZE )
		{
			avail = WAVE_CHUNK_SIZE;
		}
		short* chunk2 = _waveFile.GetChunk(_wavePosition,_wavePosition + avail);
		alBufferData(_buffers[1], _format, chunk2, avail*_waveFile.GetNumChannels()*2, _waveFile.GetSampleRate());
		delete[] chunk2;
		_wavePosition += avail;
		alSourceQueueBuffers(_source, 2, _buffers );
	}
	else if( _fileFormat == FORMAT_MP3 )
	{
		bool result = FillBuffer(_buffers[0]);
		if( !result )
		{
			return false;
		}
		result = FillBuffer(_buffers[1]);
		if( !result )
		{
			return false;
		}
		alSourceQueueBuffers(_source, 2, _buffers );
	}
	else if( _fileFormat == FORMAT_WAVPACK )
	{
		int size = WAVE_CHUNK_SIZE * this->GetChannels();
		int32_t* data = new int32_t[size];
		short* shortData = new short[size];
		uint32_t numUnpacked = WavpackUnpackSamples(_wavpackContext, data, WAVE_CHUNK_SIZE);
		int dataSize = numUnpacked * sizeof(short) * this->GetChannels();
		int rate = GetRate();
		for( unsigned int i = 0; i < numUnpacked * this->GetChannels(); i++ )
		{
			shortData[i] = data[i];
		}
		alBufferData(_buffers[0], _format, shortData, dataSize, rate);
		numUnpacked = WavpackUnpackSamples(_wavpackContext, data, WAVE_CHUNK_SIZE);
		for( unsigned int i = 0; i < numUnpacked * this->GetChannels(); i++ )
		{
			shortData[i] = data[i];
		}
		dataSize = numUnpacked * sizeof(short) * this->GetChannels();
		alBufferData(_buffers[1], _format, shortData, dataSize, rate);
		alSourceQueueBuffers(_source, 2, _buffers);
		delete[] data;
		delete[] shortData;
	}

    alSourcePlay(_source);

    return true;
}

bool MusicStream::FillBuffer(ALuint buffer)
{
	bool result = false;
	unsigned char* chunk = new unsigned char[BUFFER_SIZE];
	memset(chunk, 0, BUFFER_SIZE);
	int numDone = _audioFile->FillBuffer(chunk, BUFFER_SIZE);
	if( numDone > 0 )
	{
		alBufferData(buffer, _format, chunk, numDone, _audioFile->GetSampleRate());
		result = true;
	}
	delete[] chunk;
    Check();
	return result;
}

bool MusicStream::DecodeSpeex(ALuint buffer)
{
	int chunkSize;
	char cbits[256];
	float* output = new float[_speexFrameSize];
	short* out = new short[_speexFrameSize];
	int numRead = fread(&chunkSize, sizeof(int), 1, _musicFile);
	numRead = fread(cbits, 1, chunkSize, _musicFile);
	//speex_bits_read_from(&_bits, cbits, chunkSize);
	//speex_decode(_speexDecoderState, &_bits, output);
	for( int i = 0; i < _speexFrameSize; i++ )
	{
		out[i] = output[i];
	}
	alBufferData(buffer, AL_FORMAT_STEREO16, out, _speexFrameSize*2, 44100);
	return true;
}

bool MusicStream::PausePlayback()
{
    if( _playState != PAUSED )
    {
        printf("Setting MusicStream play state to PAUSED.\n");
	    _playState = PAUSED;
        alSourcePause( _source );
    }
    else if( _playState == PAUSED )
    {
        printf("Setting MusicStream play state to PLAYING.\n");
        _playState = PLAYING;
        alSourcePlay( _source );
    }
    // Do nothing if we are in stopped state.
	return true;
}

bool MusicStream::Playing()
{
    ALenum state;

    alGetSourcei(_source, AL_SOURCE_STATE, &state);

    bool retval = (state == AL_PLAYING);
    if( state == true )
    {
        QMessageBox(QMessageBox::Information, "Playing is true.", "Playing", QMessageBox::Ok);
    }
    return retval;
}

bool MusicStream::IsPlaying()
{
	return ( _playState == PLAYING );
}

bool MusicStream::IsPaused()
{
    return ( _playState == PAUSED );
}

bool MusicStream::Update()
{
    int processed = 0;
    bool result = true;

    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);

    while(processed--)
    {
        ALuint buffer;

        alSourceUnqueueBuffers(_source, 1, &buffer);
        Check();

		if( _fileFormat == FORMAT_VORBIS )
		{
			bool result = FillBuffer(buffer);
			if( !result )
			{
				return false;
			}
			alSourceQueueBuffers(_source, 1, &buffer );
		}
		else if( _fileFormat == FORMAT_WAVPACK )
		{
			int size = WAVE_CHUNK_SIZE * this->GetChannels(); // 8820 samples x 2 channels = 17640 ints of data samples for stereo.
			int32_t* data = new int32_t[size]; // 17640 shorts of data for 8820 stereo samples
			short* shortData = new short[size];
			uint32_t numUnpacked = WavpackUnpackSamples(_wavpackContext, data, WAVE_CHUNK_SIZE);
			if( numUnpacked < 1 )
			{
				result = false;
			}
			else
			{
				for( unsigned int i = 0; i < numUnpacked * this->GetChannels(); i++ )
				{
					shortData[i] = data[i];
				}
				int dataSize = numUnpacked * sizeof(short) * this->GetChannels();
				int rate = GetRate();
				alBufferData(buffer, _format, shortData, dataSize, rate);
				alSourceQueueBuffers(_source, 1, &buffer);
			}
			delete[] data;
			delete[] shortData;

		}
		else if( _fileFormat == FORMAT_WAVE )
		{
			unsigned int totalSamples = _waveFile.GetNumSamples();
			int avail = totalSamples - _wavePosition;
			if( avail < 2 )
			{
				result = false;
			}
			else if( avail > WAVE_CHUNK_SIZE )
			{
				avail = WAVE_CHUNK_SIZE;
			}
			short* chunk = _waveFile.GetChunk(_wavePosition,_wavePosition + avail);
			alBufferData(buffer, _format, chunk, avail*_waveFile.GetNumChannels()*2, _waveFile.GetSampleRate());
			delete[] chunk;
			_wavePosition += avail;
			alSourceQueueBuffers(_source, 1, &buffer );
		}
		else if( _fileFormat == FORMAT_MP3 )
		{
			bool result = FillBuffer(buffer);
			if( !result )
			{
				return false;
			}
			alSourceQueueBuffers(_source, 1, &buffer );
		}
    }

    return result;
}

void MusicStream::Empty()
{
    int queued;

    alGetSourcei(_source, AL_BUFFERS_QUEUED, &queued);

    while(queued--)
    {
        ALuint buffer;

        alSourceUnqueueBuffers(_source, 1, &buffer);
        Check();
    }
}

void MusicStream::Check()
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
	{
		//MessageBox( NULL, "OpenAL error was raised", "ERROR", MB_OK );
	}
}

int MusicStream::Play()
{
    if( _playState == PAUSED )
    {
        alSourcePlay( _source );
    }
    printf("Setting MusicStream play state to PLAYING.\n");
	_playState = PLAYING;

	return 0;
}

int MusicStream::PlayFile(QString file)
{
	if( !Open(file) )
	{
		return -1;
	}
    printf("Setting MusicStream play state to PLAYING.\n");
	_playState = PLAYING;

	return 0;
}

void MusicStream::Stop()
{
    printf("Setting MusicStream play state to STOPPED.\n");
	_playState = STOPPED;
}

void MusicStream::run()
{
    printf("Music playback thread started.\n");
	while( 1 )
	{
		if( !(_playState == PLAYING) )
		{
#ifdef WIN32
			Sleep( 50 );
#else
            usleep(50000);
#endif
		}
		else
		{
			if(!Playback())
			{
#ifdef WIN32
				MessageBox( NULL, "Song Refused to Play", "ERROR", MB_OK );
#endif
				return;
			}
			while(Update() && (_playState == PLAYING || _playState == PAUSED) )
			{
				if(!Playing() && _playState != PAUSED )
				{
					if( !Playback() )
					{
#ifdef WIN32
						MessageBox( NULL, "Song abruptly stopped", "ERROR", MB_OK );
#endif
						return;
					}
					else
					{
						//MessageBox( NULL, "Ogg stream was interrupted", "ERROR", MB_OK );
					}
				}
#ifdef WIN32
				Sleep(1);
#else
                usleep(1000);
#endif
			}
            printf("Setting MusicStream play state to STOPPED.\n");
			_playState = STOPPED;
		}
	}
    printf("Music playback thread exiting.\n");
	return;
}

void MusicStream::SetVolume( float volume )
{
    if( volume > 1.0f )
    {
        volume = 1.0f;
    }

    alSourcef( _source, AL_GAIN, volume );
}

ALuint MusicStream::GetOpenALFormatFromFile(WaveFile* file)
{
	int channels = file->GetNumChannels();

	// TODO: FIXME
	// Note: Any 32-bit .wav file that is loaded passes through this function
	// and is assigned a 16-bit OpenAL format and sounds terrible when played.
	// We need to take action to fix this.

	if( channels == 1 )
	{
		return AL_FORMAT_MONO16;
	}
	else if( channels == 2)
	{
		return AL_FORMAT_STEREO16;
	}

	return AL_FORMAT_STEREO16;
}
