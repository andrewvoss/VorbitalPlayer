#ifndef _VORBITALDLG_H_
#define _VORBITALDLG_H_

#include "MusicStream.h"
#include "PlaylistThread.h"
#include "AL/al.h"
#include "AL/alc.h"

#include <QDialog>
#include <QThread>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QListWidget>
#include <QDateTime>

#define SYMBOL_VorbitalDlg_TITLE _("Vorbital Player")
#define SYMBOL_VorbitalDlg_SIZE QSize(480, 400)

class VorbitalDlg;

// TODO:
// setAcceptDrops(true)
// dropEvent(QDropEvent* event);
// dragEnterEvent(QDragEnterEvent* event);
// etc, see here:
// http://qt-project.org/doc/qt-4.8/dnd.html

/*class VorbitalDropTarget: public QFileDropTarget
{
public:
	VorbitalDropTarget( VorbitalDlg* dialog ) { _dialog = dialog; }
	virtual ~VorbitalDropTarget() {}
	//virtual bool OnDropFiles(QCoord x, QCoord y, const QArrayString& filenames);
private:
	VorbitalDlg* _dialog;
};*/

class VorbitalDlg: public QDialog/*, public QThread*/
{
    Q_OBJECT
public:
    VorbitalDlg( );
    ~VorbitalDlg( );
    void CreateControls();
    //bool OnDropFiles( const QArrayString& filename );
    void LoadFile( QString& filename );
    void OnMouseWheel();
    void OnPlaylistDoubleClick();
    void OnListPosition();
    void OnQuit(QCloseEvent& event);
    void OnRightClick();
    void ShowFileInfo(int index);
    static bool ShowToolTips();
    QString ExtractFilename(const QString& filename);
    void LoadAlbumArt(const QString& filename);
    void LoadSettings();
    void SaveSettings();
    QListWidget* GetPlaylist() { return _lstPlaylist; }
    MusicStream* GetMusicStream() { return _musicStream; }
    void SetIncrementNeeded(bool value) { _incrementNeeded = value; }
    bool GetIncrementNeeded() { return _incrementNeeded; }
    bool GetRandomize() { return _randomize; }
    void SetListPosition(int value) { _listPosition = value; }
    int GetListPosition() { return _listPosition; }
    int GetPlayState() { return _playState; }
    void SetPlayState(int value) { _playState = value; printf("PlayState changed to %d\n", value); }
    void UpdateTime();
    void PausedUpdateTime();
    void ResetTime();
    void UpdateNumChannels(int channels);
    void UpdateBitrate(int bitrate);
    void UpdateSampleRate(int samplerate);
public slots:
    void OnNumChannels(int value);
    void OnBitrate(int bitrate);
    void OnSampleRate(int rate);
    void OnTime(int seconds);
    void OnButtonBrowseClick();
    void OnButtonBrowseFolderClick();
    void OnButtonPlayClick();
    void OnButtonStopClick();
    void OnButtonForwardClick();
    void OnButtonReverseClick();
    void OnButtonPauseClick();
    void OnButtonRemoveClick();
    void OnButtonSettingsClick();
    void OnButtonClearClick();
    void OnButtonRandomizeClick();
    void OnAbout();
    void OnVolume(int value);
private:
    ALCdevice *_device;
    ALCcontext *_context;
    QPushButton* _btnBrowse;
    QPushButton* _btnBrowseFolder;
    QPushButton* _btnPlay;
    QPushButton* _btnStop;
    QPushButton* _btnForward;
    QPushButton* _btnReverse;
    QPushButton* _btnPause;
    QPushButton* _btnRemove;
    QPushButton* _btnClear;
    QPushButton* _btnAbout;
    QPushButton* _btnSettings;
    QPushButton* _btnRandomize;
    QLabel* _txtSampleRate;
    QLabel* _txtVersion;
    QLabel* _txtBitRate;
    QLabel* _txtChannels;
    QLabel* _txtComment;
    QLabel* _txtTime;
    QListWidget* _lstPlaylist;
    QSlider* _volumeSlider;
	QLabel* _albumArt;
    //std::list<QString *> _playlist;
    MusicStream* _musicStream;
    int _listPosition;
    int _playState;
    bool _incrementNeeded;
    bool _randomize;
	bool _menuDoubleClicked;
	bool _done;
    QDateTime _lastTimeUpdate;
    int _timeElapsed;
    QIcon _icon;
    PlaylistThread* _playlistThread;
signals:
    void numchannelsChanged(int channels);
    void bitrateChanged(int bitrate);
    void bitdepthChanged(int bitdepth);
    void samplerateChanged(int samplerate);
    void timeChanged(int time);
};

#endif
