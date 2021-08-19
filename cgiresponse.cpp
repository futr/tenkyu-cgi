#include "cgiresponse.h"
#include <QUrlQuery>
#include <QTextStream>
#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QtMath>
CGIResponse::CGIResponse(QObject *parent) : QObject(parent)
{

}

void CGIResponse::doResponse()
{
    auto query = qEnvironmentVariable( "QUERY_STRING" );
    auto uq = QUrlQuery( query );

    // Parse query string
    double radius = uq.queryItemValue( "radius" ).toDouble();
    double mag    = uq.queryItemValue( "mag" ).toDouble();
    double starSize = uq.queryItemValue( "ssize" ).toDouble();
    double starNameSize = uq.queryItemValue( "snsize" ).toDouble();
    double consteNameSize = uq.queryItemValue( "cnsize" ).toDouble();
    double messierNameSize = uq.queryItemValue( "mnsize" ).toDouble();
    double infoStrSize = uq.queryItemValue( "infosize" ).toDouble();
    int lat   = uq.queryItemValue( "lat" ).toDouble();
    int eng   = uq.queryItemValue( "eng" ).toUInt();
    int raSplit = uq.queryItemValue( "split" ).toUInt();
    int deRep = uq.queryItemValue( "derep" ).toUInt();
    int southOffX = uq.queryItemValue( "sofx" ).toUInt();
    int southOffY = uq.queryItemValue( "sofy" ).toUInt();
    int mirror = uq.queryItemValue( "mir" ).toUInt();
    int drawName    = uq.queryItemValue( "name" ).toUInt();
    int drawConste  = uq.queryItemValue( "conste" ).toUInt();
    int drawMessier = uq.queryItemValue( "messier" ).toUInt();
    int drawObsLine = uq.queryItemValue( "obsline" ).toUInt();

    // roundup
    eng = qMax( 0, eng );
    mirror = qMax( 0, mirror );
    drawName = qMax( 0, drawName );
    drawConste = qMax( 0, drawConste );
    drawMessier = qMax( 0, drawMessier );
    drawObsLine = qMax( 0, drawObsLine );

    starNameSize = qMax( 0.0, starNameSize );
    consteNameSize = qMax( 0.0, consteNameSize );
    messierNameSize = qMax( 0.0, messierNameSize );
    infoStrSize = qMax( 0.0, infoStrSize );

    if ( raSplit < 1 || raSplit > 64 ) {
        raSplit = cp.raSplit;
    }

    if ( deRep <= 1 ) {
        deRep = 1;
    } else {
        deRep = 2;
    }

    if ( radius < 10 || radius > 300 ) {
        radius = cp.radius;
    }

    if ( starSize <= 0 || starSize > 100 ) {
        starSize = cp.starSize;
    }
    if ( lat > 90 ) {
        lat = 90;
    } else if ( lat < -90 ) {
        lat = -90;
    }

    if ( mag > 8 ) {
         mag = 8;
    } else if ( mag < 0 ) {
        mag = 0;
    }

    southOffX = qMin(  300, southOffX );
    southOffX = qMax( -300, southOffX );
    southOffY = qMin(  300, southOffY );
    southOffY = qMax( -300, southOffY );

    cp.radius = radius;
    cp.maxMagnitude = mag;
    cp.obsLatitude = lat;
    cp.useAlphabetText = eng ? true : false;
    cp.deRepeatCount = deRep;
    cp.southOffsetMm = QPointF( southOffX, southOffY );
    cp.starSize = starSize;
    cp.inv = mirror ? true : false;
    cp.printStarNames = drawName ? true : false;
    cp.printMessierName = cp.printStarNames;
    cp.printConsteName  = cp.printStarNames;
    cp.printConstellations = drawConste ? true : false;
    cp.printMessiers = drawMessier ? true : false;
    cp.printObsPointLine = drawObsLine ? true : false;
    cp.starNamePoint = starNameSize;
    cp.consteNamePoint = consteNameSize;
    cp.messierNamePoint = messierNameSize;
    cp.infoStrPoint = infoStrSize;

    // Generate pdf
    QBuffer buf;
    buf.open( QIODevice::WriteOnly );

    cp.openStarData( ":data/" );
    cp.startWritePDF( QString(), &buf );

    auto contentDisposition = QString( "inline; filename=\"%1\"" ).arg( cp.generateFileName( true ) );
    auto pdfSize = buf.size();

    // Output
    QFile ofp;
    ofp.open( stdout, QIODevice::WriteOnly );
    QTextStream ts( &ofp );

    if ( pdfSize == 0 ) {
        // Error
        QString errorStr = "<!DOCTYPE html><html><body><p>Error</p></body></html>\r\n";
        QByteArray errorByte = errorStr.toUtf8();

        // Output Header
        ts << "Content-type: text/html; charset=UTF-8\r\n";
        ts << "Content-Length: " << errorByte.size() << "\r\n";
        ts << "\r\n";
        ts << errorByte;
        ts.flush();
    } else {
        // Output Header
        ts << "Content-type: application/pdf\r\n";
        ts << "Content-Disposition: " << contentDisposition << "\r\n";
        ts << "Content-Length: " << pdfSize << "\r\n";
        ts << "\r\n";
        ts.flush();

        // Output pdf
        ofp.write( buf.data() );
        ofp.flush();
    }

    // Send quit
    QCoreApplication::quit();
}
