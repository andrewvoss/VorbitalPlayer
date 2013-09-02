#ifndef _PLAYLISTTHREAD_H_
#define _PLAYLISTTHREAD_H_

#include "MusicStream.h"
#include <QThread>

class VorbitalDlg;

/**
* The PlaylistThread is the main "controller" for the application. It monitors the play
* state and playlist and sends updates to the GUI and commands to the playback engine.
*/
class PlaylistThread: public QThread
{
    Q_OBJECT
public:
    PlaylistThread(VorbitalDlg* dlg);
    ~PlaylistThread();
    void run();
    void stop();
private:
    VorbitalDlg* _dlg;
    bool _done;
};

#endif
