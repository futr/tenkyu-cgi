#include <QGuiApplication>
#include <QFontDatabase>
#include "cgiresponse.h"

// https://stackoverflow.com/questions/40313476/qpainterdrawtext-seg-faults-in-initializedb
// sudo -u www-data ./tenkyu-cgi -platform offscreen 2> /dev/null
// https://stackoverflow.com/questions/17979185/qt-5-1-qapplication-without-display-qxcbconnection-could-not-connect-to-displ
// https://stackoverflow.com/questions/49135648/qguiapplication-without-display-using-qpainter-and-qpdfwriter
// Force -platform offscreen
// strace -f -t -e trace=file

int main(int argc, char *argv[])
{
    QList<QByteArray> argList;

    for ( int i = 0; i < argc; i++ ) {
        argList << argv[i];
    }

    if ( !argList.contains( "-platform" ) ) {
        argList << "-platform" << "offscreen";
    }

    // Regenerate args
    int newArgc = argList.count();
    char **newArgv = new char *[newArgc];

    for ( int i = 0; i < newArgc; i++ ) {
        newArgv[i] = argList[i].data();
    }

    // Setup font
    // QFontDatabase::addApplicationFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    // qputenv("QT_QPA_FONTDIR", "/usr/share/fonts/truetype/dejavu/");

    QGuiApplication a(newArgc, newArgv);

    CGIResponse cgi;

    QMetaObject::invokeMethod( &cgi, &CGIResponse::doResponse, Qt::QueuedConnection );

    return a.exec();
}
