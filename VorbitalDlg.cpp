#include "VorbitalDlg.h"
#include "SettingsDlg.h"
#ifdef WIN32
#include "alut.h"
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <QFile>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QBitmap>
#include <QSettings>
#include <QStringList>
#include <QMetaMethod>
#include <QCloseEvent>
#include <QDebug>

#include "add.xpm"
#include "folder.xpm"
#include "forward.xpm"
//#include "gear.xpm"
#include "normal.xpm"
#include "null.xpm"
#include "pause.xpm"
#include "play.xpm"
#include "question.xpm"
#include "random.xpm"
#include "remove.xpm"
#include "reverse.xpm"
#include "stop.xpm"

VorbitalDlg::~VorbitalDlg()
{
}

/*!
 * VorbitalDlg creator
 */
VorbitalDlg::VorbitalDlg( )
{
    qDebug() << "VorbitalDlg Create.";
	_done = false;
	// OpenAL Initialization
#ifdef WIN32
	alutInit(NULL, 0);
#else
    _device = alcOpenDevice(NULL);
    _context = alcCreateContext(_device, NULL);
    alcMakeContextCurrent(_context);
#endif
	alGetError();
	// Initialize position of the Listener.
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
	// Velocity of the Listener.
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);

	_listPosition = 0;
	_musicStream = NULL;
    _btnBrowse = NULL;
    _btnBrowseFolder = NULL;
    _btnPlay = NULL;
	_btnStop = NULL;
	_btnPause = NULL;
	_btnForward = NULL;
	_btnReverse = NULL;
    _btnClear = NULL;
	_btnRemove = NULL;
	_btnSettings = NULL;
	_btnAbout = NULL;
	_btnRandomize = NULL;
    _txtSampleRate = NULL;
    _txtVersion = NULL;
    _txtBitRate = NULL;
    _txtChannels = NULL;
	_txtComment = NULL;
	_txtTime = NULL;
    _volumeSlider = NULL;
    _timeElapsed = 0;
    qDebug() << "Setting VorbitalDlg play state to STOPPED.";
	_playState = STOPPED;
	_incrementNeeded = true;
    _randomize = false;
	_menuDoubleClicked = false;
	srand((unsigned)time(0));
    // TODO: Enable drag and drop.
    //SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
	//_dropTarget = new VorbitalDropTarget(this);
	//QWindow::SetDropTarget( _dropTarget );
    CreateControls();
    // Register the meta type for saving and loading the playlist.
    qRegisterMetaType<QList <QString> >("QList<QString>");
    qRegisterMetaTypeStreamOperators<QList <QString> >("QList<QString>");
	LoadSettings();
    QIcon icon("vorbital.ico");
	setWindowIcon(icon);
    setWindowTitle("Vorbital Player");
	// Start up the playlist thread.
    _playlistThread = new PlaylistThread(this);
	_playlistThread->start();
}

void VorbitalDlg::LoadSettings()
{
	QSettings* configData = new QSettings("Zeta Centauri", "Vorbital Player");
    qDebug() << "LoadSettings loading file " << configData << ".";

	// Randomizer setting.
	_randomize = configData->value( "randomize" ).toInt();
	if( _randomize )
	{
		_btnRandomize->setIcon(QPixmap(random_xpm));
	}
	else
	{
		_btnRandomize->setIcon(QPixmap(normal_xpm));
	}
	// Volume setting.
	_volumeSlider->setValue((configData->value( "volume" )).toInt());
    int volume = _volumeSlider->value();
    float actualVol = (float)volume / 100.0f;
	if( _musicStream != NULL )
	{
		_musicStream->SetVolume( actualVol );
	}
	// Window size.  Default size is 386x360 if it hasn't been changed.
	int sizex = (configData->value( "sizex" )).toInt();
	int sizey = (configData->value( "sizey" )).toInt();
	if( sizex > 0 && sizey > 0 )
	{
        setFixedSize(sizex, sizey);
        setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
	}
    QVariantList variantList;
    variantList = (configData->value( "playlist" )).toList();
    foreach(QVariant v, variantList)
    {
        QFileInfo info(v.toString());
        QListWidgetItem* item = new QListWidgetItem(info.baseName());
        item->setData(Qt::UserRole, QVariant(info.absoluteFilePath()));
        _lstPlaylist->addItem(item);
    }
    qDebug() << "Loaded Settings: Randomize =" << _randomize << ", Volume =" << volume << ", Width =" << sizex <<
        ", Height =" << sizey << ", Playlist =" << variantList.count() << " items.";
	delete configData;
}

void VorbitalDlg::SaveSettings()
{
    qDebug() << "SaveSettings.";
	QSettings* configData = new QSettings("Zeta Centauri", "Vorbital Player");
	configData->setValue("randomize", _randomize);
	configData->setValue("volume", _volumeSlider->value());
	QSize wsize = size();
	configData->setValue("sizex", wsize.width());
	configData->setValue("sizey", wsize.height());
    QList<QString> playlistItems;
    for( int i = 0; i < _lstPlaylist->count(); i++ )
    {
        QListWidgetItem* item = _lstPlaylist->item(i);
        QVariant variant = item->data(Qt::UserRole);
        QString filename = variant.toString();
        qDebug() << "Saving Playlist Item: " << filename << "'.";
		playlistItems << filename;
    }
    configData->setValue("playlist", QVariant::fromValue<QList <QString> >(playlistItems));
    configData->sync();
    qDebug() << "Saved Settings: Randomize =" << _randomize << ", Volume =" << _volumeSlider->value() <<
        ", Width =" << wsize.width() << ", Height =" << wsize.height() << ", Playlist =" << playlistItems.count() << " items.";
	delete configData;
}

/*!
 * Control creation for VorbitalDlg
 */
void VorbitalDlg::CreateControls()
{
    qDebug() << "CreateControls.";

    QVBoxLayout* rootLayout = new QVBoxLayout();
    setLayout(rootLayout);

    QHBoxLayout* firstRowLayout = new QHBoxLayout();
    rootLayout->addLayout(firstRowLayout);

    firstRowLayout->setContentsMargins(QMargins(0,0,0,0));
    firstRowLayout->setSpacing(2);

	_btnBrowse = new QPushButton( this );
    _btnBrowse->setIcon(QPixmap(add_xpm));
	_btnBrowse->setToolTip("Add file to playlist.");
    connect(_btnBrowse, SIGNAL(released()), this, SLOT(OnButtonBrowseClick()));
    firstRowLayout->addWidget(_btnBrowse);

    _btnBrowseFolder = new QPushButton( this );
    _btnBrowseFolder->setIcon(QPixmap(folder_xpm));
	_btnBrowseFolder->setToolTip("Add folder to playlist.");
    connect(_btnBrowseFolder, SIGNAL(released()), this, SLOT(OnButtonBrowseFolderClick()));
    firstRowLayout->addWidget(_btnBrowseFolder);

    _btnRemove = new QPushButton( this );
    _btnRemove->setIcon(QPixmap(remove_xpm));
	_btnRemove->setToolTip("Remove selected item from playlist.");
    connect(_btnRemove, SIGNAL(released()), this, SLOT(OnButtonRemoveClick()));
    firstRowLayout->addWidget(_btnRemove);

    _btnClear = new QPushButton( this );
    _btnClear->setIcon(QPixmap(null_xpm));
	_btnClear->setToolTip("Clear playlist.");
    connect(_btnClear, SIGNAL(released()), this, SLOT(OnButtonClearClick()));
    firstRowLayout->addWidget(_btnClear);

	_btnReverse = new QPushButton( this );
    _btnReverse->setIcon(QPixmap(reverse_xpm));
	_btnReverse->setToolTip("Previous track.");
    connect(_btnReverse, SIGNAL(released()), this, SLOT(OnButtonReverseClick()));
    firstRowLayout->addWidget(_btnReverse);

	_btnPause = new QPushButton( this );
    _btnPause->setIcon(QPixmap(pause_xpm));
	_btnPause->setToolTip("Pause.");
    connect(_btnPause, SIGNAL(released()), this, SLOT(OnButtonPauseClick()));
    firstRowLayout->addWidget(_btnPause);

	_btnStop = new QPushButton( this );
    _btnStop->setIcon(QPixmap(stop_xpm));
	_btnStop->setToolTip("Stop.");
    connect(_btnStop, SIGNAL(released()), this, SLOT(OnButtonStopClick()));
    firstRowLayout->addWidget(_btnStop);

	_btnPlay = new QPushButton( this );
    _btnPlay->setIcon(QPixmap(play_xpm));
	_btnPlay->setToolTip("Play.");
    connect(_btnPlay, SIGNAL(released()), this, SLOT(OnButtonPlayClick()));
    firstRowLayout->addWidget(_btnPlay);

    _btnForward = new QPushButton( this );
    _btnForward->setIcon(QPixmap(forward_xpm));
	_btnForward->setToolTip("Next track.");
    connect(_btnForward, SIGNAL(released()), this, SLOT(OnButtonForwardClick()));
    firstRowLayout->addWidget(_btnForward);

    _btnRandomize = new QPushButton( this );
    _btnRandomize->setIcon(QPixmap(normal_xpm));
	_btnRandomize->setToolTip("Turn shuffle on or off.");
    connect(_btnRandomize, SIGNAL(released()), this, SLOT(OnButtonRandomizeClick()));
    firstRowLayout->addWidget(_btnRandomize);

	_btnAbout = new QPushButton( this );
    _btnAbout->setIcon(QPixmap(question_xpm));
	_btnAbout->setToolTip("About Vorbital Player");
    connect(_btnAbout, SIGNAL(released()), this, SLOT(OnAbout()));
    firstRowLayout->addWidget(_btnAbout);

    QHBoxLayout* secondRowLayout = new QHBoxLayout();
    rootLayout->addLayout(secondRowLayout);

    secondRowLayout->insertSpacing(6, 6);

	QLabel* itemStaticText16 = new QLabel(this);
    itemStaticText16->setText("SR:");
	itemStaticText16->setToolTip("Sample rate.");
    secondRowLayout->addWidget(itemStaticText16);

    _txtSampleRate = new QLabel(this);
    _txtSampleRate->setText("0");
    secondRowLayout->addWidget(_txtSampleRate);

	secondRowLayout->insertSpacing(4, 4);

	QLabel* itemStaticText20 = new QLabel(this);
    itemStaticText20->setText("BR:");
	itemStaticText20->setToolTip("Bit rate.");
    secondRowLayout->addWidget(itemStaticText20);

    _txtBitRate = new QLabel(this);
    _txtBitRate->setText("0");
    secondRowLayout->addWidget(_txtBitRate);

    secondRowLayout->insertSpacing(4, 4);

    _txtChannels = new QLabel(this);
    _txtChannels->setText("Stereo");
    secondRowLayout->addWidget(_txtChannels);

	secondRowLayout->insertSpacing(10, 10);

    _volumeSlider = new QSlider(Qt::Horizontal, this);
    _volumeSlider->setMinimum(0);
    _volumeSlider->setMaximum(100);
    _volumeSlider->setValue(0);
    _volumeSlider->setMaximumSize(100, 24);
    connect(_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(OnVolume(int)));
    secondRowLayout->addWidget(_volumeSlider);

	secondRowLayout->insertSpacing(10, 10);

	_txtTime = new QLabel(this);
    _txtTime->setText("0:00");
    secondRowLayout->addWidget(_txtTime);

	QBitmap* bitmap = new QBitmap(120, 120);
	_albumArt = new QLabel(this);
    _albumArt->setPixmap(*bitmap);
	rootLayout->addWidget(_albumArt);
	_albumArt->setVisible(false);

	_lstPlaylist = new QListWidget( this );
	rootLayout->addWidget(_lstPlaylist);
	connect(_lstPlaylist, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnPlaylistDoubleClick(QListWidgetItem*)), Qt::AutoConnection);

    connect(this, SIGNAL(bitrateChanged(int)), this, SLOT(OnBitrate(int)), Qt::AutoConnection);
    connect(this, SIGNAL(numchannelsChanged(int)), this, SLOT(OnNumChannels(int)), Qt::AutoConnection);
    connect(this, SIGNAL(samplerateChanged(int)), this, SLOT(OnSampleRate(int)), Qt::AutoConnection);
    connect(this, SIGNAL(timeChanged(int)), this, SLOT(OnTime(int)), Qt::AutoConnection);
}

void VorbitalDlg::OnBitrate(int value)
{
  qDebug() << "Bitrate changed to " << value << ".";
  _txtBitRate->setText(QString("%1").arg(value));
}

void VorbitalDlg::OnListPosition()
{
  //_lstPlaylist->SetSelection(event.GetString());
}

void VorbitalDlg::OnNumChannels(int data)
{
  qDebug() << "Number of channels changed to " << data << ".";
  if( data == 1 )
  {
    _txtChannels->setText("Mono");
  }
  else
  {
    _txtChannels->setText("Stereo");
  }
}

void VorbitalDlg::OnTime(int seconds)
{
  QString label;
  if( (seconds % 60) > 9 )
  {
 	  label = QString("%1:%2").arg(seconds / 60).arg(seconds % 60);
  }
  else
  {
      label = QString("%1:0%2").arg(seconds / 60).arg(seconds % 60);
  }
  _txtTime->setText(label);
}

void VorbitalDlg::OnSampleRate(int data)
{
  qDebug() << "Samplerate changed to " << data << ".";
  _txtSampleRate->setText(QString("%1").arg(data));
}

/**
* Handles right mouse clicks on the playlist.
*/
// TODO: Reimplement this, and bind it to a right click signal.
void VorbitalDlg::OnRightClick()
{
	//int index = _lstPlaylist->itemAt(<< mouse position >>);
	//if( index > -1 )
	//{
	//	ShowFileInfo(index);
	//}
}

/**
* Shows file details for a playlist item.
*/
void VorbitalDlg::ShowFileInfo(int index)
{
    QListWidgetItem* item = _lstPlaylist->item(index);
    QVariant variant = item->data(Qt::UserRole);
    QString filename = variant.toString();
	QMessageBox(QMessageBox::Information, filename, "File Location", QMessageBox::Ok);
}

bool VorbitalDlg::ShowToolTips()
{
    return true;
}

void VorbitalDlg::OnButtonBrowseClick()
{
	QStringList filenames = QFileDialog::getOpenFileNames( this, "Choose a file", ".",
		"Supported Files (*.aif *.aiff *.mp3 *.ogg *.snd *.wav *.wv)" );
//		"Supported Files (*.aif, *.aiff, *.mp3, *.ogg, *.snd, *.wav, *.wv) |*.aif;*.AIF;*.aiff;*.AIFF;*.mp3;*.MP3;*.ogg;*.OGG;*.snd;*.SND;*.wv;*.WV;*.wav;*.WAV|AIFF Files (*.aif, *.aiff) |*.aif;*.AIF;*.aiff;*.AIFF|MP3 Files (*.mp3) |*.mp3;*.MP3|Ogg Vorbis Files (*.ogg) |*.ogg;*.OGG|SND Files (*.snd) |*.snd;*.SND|Wave Files (*.wav) |*.wav;*.WAV|Wavpack Files (*.wv) |*.wv;*.WV||" );

	for( int i = 0; i < filenames.length(); i++ )
	{
        QFileInfo info(filenames[i]);
        QListWidgetItem* item = new QListWidgetItem(info.baseName());
        item->setData(Qt::UserRole, QVariant(info.absoluteFilePath()));
        _lstPlaylist->addItem(item);
	}

	if( _lstPlaylist->currentRow() < 0 )
	{
		_lstPlaylist->setCurrentRow(0);
		_listPosition = 0;
	}

	// Update the file metrics display once the file is loaded.
}

void VorbitalDlg::OnButtonBrowseFolderClick()
{
	QFileDialog fdialog( this, "Choose a directory", ".");
    QString dir = QFileDialog::getExistingDirectory(this, "Choose a directory", ".", QFileDialog::ShowDirsOnly);
    qDebug() << "Directory: " << dir << ".";
    QDir workingDirectory(dir);
    QStringList filters;
    filters << "*.wav" << "*.mp3" << "*.ogg" << "*.wv" << "*.snd" << "*.aif" << "*.aiff" /*<< "*.flac"*/;
    QFileInfoList files = workingDirectory.entryInfoList(filters, QDir::Files, QDir::Name);
	for( int i = 0; i < files.count(); i++ )
	{
        QListWidgetItem* item = new QListWidgetItem(files[i].baseName());
        item->setData(Qt::UserRole, QVariant(files[i].absoluteFilePath()));
        _lstPlaylist->addItem(item);
	}

	if( _lstPlaylist->currentRow() < 0 )
	{
		_lstPlaylist->setCurrentRow(0);
		_listPosition = 0;
	}

	// Update the file metrics display once the file is loaded.
}

void VorbitalDlg::OnButtonStopClick()
{
    qDebug() << "Stop clicked.";
    qDebug() << "Setting VorbitalDlg play state to STOPPED.";
	_playState = STOPPED;

	if( _musicStream )
	{
		_musicStream->Stop();
	}
}

void VorbitalDlg::OnButtonPauseClick()
{
    qDebug() << "Pause clicked.";
	if( _playState == PLAYING )
	{
		_musicStream->PausePlayback();
	}
}

void VorbitalDlg::OnButtonSettingsClick()
{
	SettingsDlg* dlg = new SettingsDlg(this);
    dlg->setModal(true);
	dlg->show();
	delete dlg;
}

void VorbitalDlg::OnButtonReverseClick()
{
	if( _lstPlaylist->count() < 1 )
	{
		return;
	}

	bool playing = false;

	if( _musicStream )
	{
		if( _musicStream->IsPlaying() || _musicStream->IsPaused() )
		{
			_musicStream->Stop();
			playing = true;
		}
	}

	// Set play pointer to next item.
	if( !_randomize )
	{
		--_listPosition;
	}
	else
	{
		_listPosition = rand() % _lstPlaylist->count();
	}
	_incrementNeeded = false;
	// Wrap list if necessary.
	if( _listPosition < 0 )
	{
		_listPosition =  _lstPlaylist->count() - 1;
	}
	_lstPlaylist->setCurrentRow( _listPosition );

	if( playing == true )
	{
		OnButtonPlayClick();
	}
}

void VorbitalDlg::OnButtonForwardClick()
{
	if( _lstPlaylist->count() < 1 )
	{
		return;
	}

	bool playing = false;

	if( _musicStream )
	{
		if( _musicStream->IsPlaying() || _musicStream->IsPaused() )
		{
			_musicStream->Stop();
			playing = true;
		}
	}

	// Set play pointer to next item.
	if( !_randomize )
	{
		++_listPosition;
	}
	else
	{
		_listPosition = rand() % _lstPlaylist->count();
	}
	_incrementNeeded = false;
	// Wrap list if necessary.
	if( _listPosition >= _lstPlaylist->count() )
	{
		_listPosition = 0;
	}
	_lstPlaylist->setCurrentRow( _listPosition );

	if( playing == true )
	{
		OnButtonPlayClick();
	}
}

void VorbitalDlg::OnButtonRandomizeClick()
{
	_randomize = !_randomize;
	if( _randomize )
	{
        //bmpRandom.SetMask(new QMask(bmpRandom, *wxWHITE));
		_btnRandomize->setIcon( QPixmap(random_xpm) );
	}
	else
	{
        //bmpNormal.SetMask(new QMask(bmpNormal, *wxWHITE));
		_btnRandomize->setIcon( QPixmap(normal_xpm) );
	}
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_REMOVE
 */
void VorbitalDlg::OnButtonRemoveClick()
{
	// Can't delete from a blank list.
	if( _lstPlaylist->count() < 1 )
	{
		return;
	}

    int removePosition = _lstPlaylist->currentRow();

	// Remove the currently selected item from the playlist, or the first item if nothing is selected.
	if( removePosition == -1 && _lstPlaylist->count() > 0 )
	{
		removePosition = 0;
	}
	QListWidgetItem* item = _lstPlaylist->takeItem( removePosition );
    delete item;

	if( _lstPlaylist->count() > 0 )
	{
		// Decrement the list position if it was at the end.
		if( _listPosition >= _lstPlaylist->count() )
		{
			--_listPosition;
		}
		if( removePosition >= _lstPlaylist->count() )
		{
			--removePosition;
		}
		_lstPlaylist->setCurrentRow( removePosition );
	}
}

void VorbitalDlg::OnButtonClearClick()
{
    _lstPlaylist->clear();
    _listPosition = 0;
    qDebug() << "Setting VorbitalDlg play state to STOPPED.";
    _playState = STOPPED;
    //OnButtonStopClick();
}

void VorbitalDlg::OnPlaylistDoubleClick(QListWidgetItem*)
{
	// Identical to clicking play, but at the specified position.
	//
	// The first time the playlist needs to advance from a double-click, it plays the
	// initially double-clicked track over again.  The commented code almost worked,
	// except that it broke double-click after a song had been playing and the playlist
	// had advanced.
	//
	// Clearly there are deeper problems with playlist advancing.
	//if(_menuDoubleClicked)
	//{
		_incrementNeeded = false;
	//}
	//else
	//{
	//	_incrementNeeded = true;
	//	_menuDoubleClicked = true;
	//}
	if( _musicStream )
	{
		_musicStream->Stop();
	}
    qDebug() << "Setting VorbitalDlg play state to STOPPED.";
	_playState = STOPPED;
	_listPosition = _lstPlaylist->currentRow();
    qDebug() << "Setting playlist position to " << _listPosition << ".";
	OnButtonPlayClick();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PLAY
 */
void VorbitalDlg::OnButtonPlayClick()
{
    qDebug() << "Play clicked.";
	if( !_musicStream )
	{
        qDebug() << "Creating new MusicStream instance.";
		_musicStream = new MusicStream(this);
		int volume = _volumeSlider->value();
		float actualVol = (float)volume / 100.0f;
		if( _musicStream != NULL )
		{
			_musicStream->SetVolume( actualVol );
		}
        qDebug() << "Playing new MusicStream instance.";
        _musicStream->start();
        _musicStream->Play();
	}

    if( _musicStream->IsPaused() )
    {
        _musicStream->PausePlayback();
    }
    else if( !_musicStream->IsPlaying() )
    {
    	_lastTimeUpdate = QDateTime::currentDateTime();
        _timeElapsed = 0;
    }

    qDebug() << "Setting VorbitalDlg play state to PLAYING.";
	_playState = PLAYING;
}

/**
 @brief  EVT_QUIT handler.
*/
void VorbitalDlg::OnQuit()
{
	SaveSettings();
	_done = true;
#ifdef WIN32
	Sleep(50);
#else
    usleep(50);
#endif
    qDebug() << "Closing OpenAL context and device.";
    alcMakeContextCurrent(NULL);
    alcDestroyContext(_context);
    alcCloseDevice(_device);
}

/**
* Shows the about box.
*/
void VorbitalDlg::OnAbout()
{
    QMessageBox::about(this, "Vorbital Player 4.0", "Vorbital Player 4.0\nCopyright 2006-2013 Zeta Centauri\nDeveloped by Jason Champion\nThe Vorbital Player is free software and may be distributed freely.\nhttp://vorbitalplayer.com?version=4.0\nhttp://vorbitalplayer.com\nVorbital uses the libogg 1.3.0, libvorbis 1.3.3, wavpack 4.60.1,\nmpg123 1.14.2, and libsndfile 1.0.25 libraries.");
}

/*
* Handles drag and drop.
*/
/*
bool VorbitalDlg::OnDropFiles(const QArrayString& filenames)
{
	if( filenames.GetCount() > 0 )
	{
		for( unsigned int i = 0; i < filenames.Count(); i++ )
		{
			_lstPlaylist->addItem( ExtractFilename(filenames[i]), new QString(filenames[i]) );
		}
	}

	if( _lstPlaylist->currentRow() < 0 )
	{
		_lstPlaylist->setCurrentRow(0);
		_listPosition = 0;
	}

	return true;
}

bool VorbitalDropTarget::OnDropFiles(QCoord x, QCoord y, const QArrayString& filenames)
{
	if( filenames.GetCount() < 1 || !_dialog )
	{
		return false;
	}
	_dialog->OnDropFiles( filenames );

	return true;
}
*/

void VorbitalDlg::LoadFile( QString& filename )
{
    int count = _lstPlaylist->count();
    if( _lstPlaylist != NULL )
    {
        QListWidgetItem* item = new QListWidgetItem(ExtractFilename(filename));
        item->setData(Qt::UserRole, QVariant(filename));
        _lstPlaylist->addItem(item);
        _lstPlaylist->setCurrentRow( count );
    }
    OnButtonPlayClick();
}

void VorbitalDlg::OnVolume(int volume)
{
    if( _musicStream )
    {
        float actualVol = (float)volume / 100.0f;
        //cout << "Setting volume to " << actualVol << endl;
        _musicStream->SetVolume( actualVol );
    }
}

/*
* Rips the extension and directory path off of a filename for prettier playlist display.
*/
QString VorbitalDlg::ExtractFilename(const QString& filename)
{
	int startPos = filename.lastIndexOf(QChar('\\'));
	if( startPos == filename.length() || startPos == -1 )
		startPos = 0;
	if( startPos != 0 )
		startPos += 1;
	int endPos = filename.lastIndexOf(QChar('.'));
	QString strng = filename.mid(startPos, endPos-startPos);
	return strng;
}

void VorbitalDlg::LoadAlbumArt(const QString& filename)
{
#ifdef WIN32
	int endPos = filename.lastIndexOf(QChar('\\'));
#else
	int endPos = filename.lastIndexOf(QChar('/'));
#endif
	QString dirname = filename.mid(0, endPos+1);
	QString artFile = QString("%1%2").arg(dirname).arg("Folder.jpg");
    qDebug() << "Loading album art file: " << artFile;
    QFile file(artFile);
	if( file.exists() )
	{
        QPixmap image;
		if( image.load(artFile) )
        {
	        image = image.scaled(QSize(120, 120), Qt::KeepAspectRatio);
		    _albumArt->setPixmap(image);
		    _albumArt->setVisible(true);
		    //Refresh();
        }
	}
	else
	{
		_albumArt->setVisible(false);
		//Refresh();
	}
}

void VorbitalDlg::UpdateTime()
{
    QDateTime currTime = QDateTime::currentDateTime();
    _timeElapsed += _lastTimeUpdate.secsTo(currTime);
    _lastTimeUpdate = currTime;
    emit timeChanged(_timeElapsed);
}

void VorbitalDlg::PausedUpdateTime()
{
    // Discard the seconds that pass while we are paused.
    _lastTimeUpdate = QDateTime::currentDateTime();
}

void VorbitalDlg::ResetTime()
{
    _timeElapsed = 0;
    _lastTimeUpdate = QDateTime::currentDateTime();
    emit timeChanged(_timeElapsed);
}

void VorbitalDlg::UpdateNumChannels(int channels)
{
    if( channels == 1 )
    {
        emit numchannelsChanged(1);
    }
    else
    {
        // Pretend that 2 or more channels is stereo.  0 channels will also show as this... if possible.
        emit numchannelsChanged(2);
    }
}

void VorbitalDlg::UpdateBitrate(int bitrate)
{
    emit bitrateChanged(bitrate);
}

void VorbitalDlg::UpdateSampleRate(int samplerate)
{
    emit samplerateChanged(samplerate);
}

void VorbitalDlg::closeEvent(QCloseEvent* event)
{
    OnQuit();
    event->accept();
}
