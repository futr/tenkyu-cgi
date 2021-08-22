#include <QGuiApplication>
#include <QFontDatabase>
#include <QThread>
#include <QTimer>
#include <QTranslator>
#include <QLibraryInfo>
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

    QGuiApplication a(newArgc, newArgv);

    QThread cgiThread( &a );
    auto cgi = new CGIResponse();

    // CGI runs on a thread
    cgi->moveToThread( &cgiThread );
    cgiThread.start();

    // Invoke CGI response
    QMetaObject::invokeMethod( cgi, &CGIResponse::doResponse, Qt::QueuedConnection );

    // Set watchdog timer
    QTimer::singleShot( 10000, &a, &QGuiApplication::quit );

    // Start main message loop
    auto ret = a.exec();

    // Stop CGI thread
    cgiThread.quit();
    cgiThread.wait( 100 );

    return ret;
}
