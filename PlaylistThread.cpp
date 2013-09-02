#include "VorbitalDlg.h"
#include "PlaylistThread.h"
#include <QMessageBox>

PlaylistThread::PlaylistThread(VorbitalDlg* dlg)
{
    _dlg = dlg;
    _done = false;
}

PlaylistThread::~PlaylistThread()
{
    stop();
}

void PlaylistThread::stop()
{
    _done = true;
}

void PlaylistThread::run()
{
    printf("Entering playlist thread.\n");
	while( !_done )
	{
		if( !(_dlg->GetPlayState() == PLAYING) )
		{
#ifdef WIN32
			Sleep( 30 );
#else
            usleep(30000);
#endif
		}
		else
		{
			while( _dlg->GetPlayState() == PLAYING )
			{
                printf("Resetting time elapsed.\n");
                _dlg->ResetTime();
                if( _dlg->GetPlaylist()->count() > 0 )
                {
				    _dlg->GetPlaylist()->setCurrentRow( _dlg->GetListPosition() );
					if( _dlg->GetIncrementNeeded() == false )
					{
						_dlg->SetIncrementNeeded(true);
					}
                }
                else
                {
                    _dlg->SetPlayState(STOPPED);
                    break;
                }
                QString filename;
                int position = _dlg->GetListPosition();
                QListWidgetItem* item = _dlg->GetPlaylist()->item(position);
                QVariant variant = item->data(Qt::UserRole);
                filename = variant.toString();
				if( filename.isEmpty() )
				{
					_dlg->SetPlayState(STOPPED);
					break;
				}
				if( filename.length() < 1 )
				{
					QMessageBox(QMessageBox::Information, QString("Nothing to play."), QString("Nothing to play."), QMessageBox::Ok);
					_dlg->SetPlayState(STOPPED);
					break;
				}
                MusicStream* stream = _dlg->GetMusicStream();
                printf("Calling stream->Open with %s\n", filename.toStdString().c_str());
				if( stream->Open(filename) )
                {
				    _dlg->GetMusicStream()->Play();
#ifdef WIN32
					Sleep(20);
#else
                    usleep(20000);
#endif
                    printf("Updating channels, bitrate, and sample rate.\n");
                    _dlg->UpdateNumChannels(_dlg->GetMusicStream()->GetChannels() );
                    _dlg->UpdateBitrate(_dlg->GetMusicStream()->GetBitrate() / 1000);
                    _dlg->UpdateSampleRate(_dlg->GetMusicStream()->GetRate());
				}
                else
                {
                    printf("Stream open failed.\n");
                    _dlg->OnButtonForwardClick();
                }

				while( _dlg->GetMusicStream()->IsPlaying() || _dlg->GetMusicStream()->IsPaused() )
				{
                    //printf("Updating time.\n");
                    if( !_dlg->GetMusicStream()->IsPaused() )
                    {
                        _dlg->UpdateTime();
                    }
                    else
                    {
                        _dlg->PausedUpdateTime();
                    }
#ifdef WIN32
				    Sleep(50);
#else
                    usleep(50000);
#endif
				}
				if( _dlg->GetPlayState() == PLAYING )
				{
					// Set play pointer to next item.  We use a goofy bool to keep from
					// getting hosed on skip forward and skip back.
					if( _dlg->GetIncrementNeeded() == false )
					{
						_dlg->SetIncrementNeeded(true);
					}
					else
					{
						if( !_dlg->GetRandomize() )
						{
							_dlg->SetListPosition(_dlg->GetListPosition() + 1);
						}
						else
						{
							_dlg->SetListPosition( rand() % _dlg->GetPlaylist()->count() );
						}
					}
					// Wrap list if necessary.
					if( _dlg->GetListPosition() >= _dlg->GetPlaylist()->count() )
					{
						_dlg->SetListPosition(0);
					}
				}
			}
            printf("Skipped while playstate == PLAYING loop.\n");
		}
	}
    printf("Exiting playlist thread.\n");
	return;
}

