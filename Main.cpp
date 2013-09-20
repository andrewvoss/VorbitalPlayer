#include <QApplication>
#include "VorbitalDlg.h"

int main( int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Vorbital Player");
    VorbitalDlg* dlg = new VorbitalDlg();
    dlg->show();
    return app.exec();
}
