#include "cgiresponse.h"
#include <QUrlQuery>
#include <QTextStream>
#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QtMath>
#include <QTranslator>
#include <QLibraryInfo>
#include <QGuiApplication>
CGIResponse::CGIResponse(QObject *parent) : QObject(parent)
{

}

void CGIResponse::doResponse()
{
    auto query = qEnvironmentVariable( "QUERY_STRING" );
    auto uq = QUrlQuery( query );

    // Parse query string
    QString localeStr = uq.queryItemValue( "l" );
    double radius = uq.queryItemValue( "radius" ).toDouble();
    double mag    = uq.queryItemValue( "mag" ).toDouble();
    double starSize = uq.queryItemValue( "ssize" ).toDouble();
    double starNameSize = uq.queryItemValue( "snsize" ).toDouble();
    double consteNameSize = uq.queryItemValue( "cnsize" ).toDouble();
    double messierNameSize = uq.queryItemValue( "mnsize" ).toDouble();
    double infoStrSize = uq.queryItemValue( "infosize" ).toDouble();
    double lat   = uq.queryItemValue( "lat" ).toDouble();
    double lon   = uq.queryItemValue( "lon" ).toDouble();
    int eng   = uq.queryItemValue( "eng" ).toUInt();
    int raSplit = uq.queryItemValue( "split" ).toUInt();
    int deRep = uq.queryItemValue( "derep" ).toUInt();
    int raRep = uq.queryItemValue( "rarep" ).toUInt();
    int southOffX = uq.queryItemValue( "sofx" ).toInt();
    int southOffY = uq.queryItemValue( "sofy" ).toInt();
    int segOffX = uq.queryItemValue( "ofx" ).toInt();
    int segOffY = uq.queryItemValue( "ofy" ).toInt();
    int mirror = uq.queryItemValue( "mir" ).toUInt();
    int drawName    = uq.queryItemValue( "name" ).toUInt();
    int drawConste  = uq.queryItemValue( "conste" ).toUInt();
    int drawMessier = uq.queryItemValue( "messier" ).toUInt();
    int drawObsLine = uq.queryItemValue( "obsline" ).toUInt();
    auto starColor = QColor( uq.queryItemValue( "sclr" ) );
    auto consteColor = QColor( uq.queryItemValue( "cclr" ) );
    auto messierColor = QColor( uq.queryItemValue( "mclr" ) );
    QString obsDateStr = uq.queryItemValue( "d" );
    QString obsTimeStr = uq.queryItemValue( "t", QUrl::FullyDecoded );
    int offsetFromUTC = uq.queryItemValue( "tz" ).toDouble();
    int drawZenith = uq.queryItemValue( "zenith" ).toUInt();

    QDate obsDate = QDate::fromString( obsDateStr, Qt::ISODate );
    QTime obsTime = QTime::fromString( obsTimeStr, "HH:mm" );

    if ( !obsDate.isValid() ) obsDate = cp.obsLocalDateTime.date();
    if ( !obsTime.isValid() ) obsTime = cp.obsLocalDateTime.time();

    // roundup
    eng = qMax( 0, eng );
    mirror = qMax( 0, mirror );
    drawName = qMax( 0, drawName );
    drawConste = qMax( 0, drawConste );
    drawMessier = qMax( 0, drawMessier );
    drawObsLine = qMax( 0, drawObsLine );

    if ( starNameSize    < 1 || starNameSize    > 50 ) starNameSize = cp.starNamePoint;
    if ( consteNameSize  < 1 || consteNameSize  > 50 ) consteNameSize = cp.consteNamePoint;
    if ( messierNameSize < 1 || messierNameSize > 50 ) messierNameSize = cp.messierNamePoint;
    if ( infoStrSize     < 1 || infoStrSize     > 50 ) infoStrSize = cp.infoStrPoint;

    if ( raSplit < 1 || raSplit > 64 ) raSplit = cp.raSplit;
    if ( raRep < 1 || raRep > 12 ) raRep = cp.raRepeatCount;

    if ( deRep <= 1 ) {
        deRep = 1;
    } else {
        deRep = 2;
    }

    if ( radius < 10 || radius > 300 ) radius = cp.radius;
    if ( starSize <= 0 || starSize > 100 ) starSize = cp.starSize;

    if ( lat > 90 ) {
        lat = 90;
    } else if ( lat < -90 ) {
        lat = -90;
    }

    if ( lon > 180 ) {
        lon = 180;
    } else if ( lon < -180 ) {
        lon = -180;
    }

    if ( mag > 8 ) {
         mag = 8;
    } else if ( mag < 0 ) {
        mag = 0;
    }

    if ( offsetFromUTC > 12 ) {
        offsetFromUTC = 12;
    } else if ( offsetFromUTC < -12 ) {
        offsetFromUTC = -12;
    }

    southOffX = qMin(  300, southOffX );
    southOffX = qMax( -300, southOffX );
    southOffY = qMin(  300, southOffY );
    southOffY = qMax( -300, southOffY );

    if ( segOffX < 0 || segOffX > 300 ) cp.segmentOffsetMm.x();
    if ( segOffY < 0 || segOffY > 300 ) cp.segmentOffsetMm.y();

    cp.radius = radius;
    cp.raSplit = raSplit;
    cp.maxMagnitude = mag;
    cp.obsLatitude = lat;
    cp.obsLongitude = lon;
    cp.useAlphabetText = eng ? true : false;
    cp.deRepeatCount = deRep;
    cp.raRepeatCount = raRep;
    cp.segmentOffsetMm = QPointF( segOffX, segOffY );
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
    cp.printObsPointZenith = drawZenith ? true : false;
    cp.obsLocalDateTime = QDateTime( obsDate, obsTime, Qt::TimeSpec::OffsetFromUTC, offsetFromUTC * 60 * 60 );

    if ( starColor.isValid() ) cp.starColor = starColor;
    if ( consteColor.isValid() ) cp.consteColor = consteColor;
    if ( messierColor.isValid() ) cp.messierColor = messierColor;

    // Setup translations
    QTranslator trans;

    if ( !localeStr.isEmpty() ) {
        QString l = localeStr;

        if ( !trans.load( QLocale( l ), QLatin1String( "tenkyu" ), QLatin1String( "_" ), QLibraryInfo::location( QLibraryInfo::TranslationsPath ), QLatin1String( ".qm" ) ) ) {
            if ( !trans.load( QLocale( l ), QLatin1String( "tenkyu" ), QLatin1String( "_" ), QLatin1String( "./translations" ), QLatin1String( ".qm" ) ) ) {
                trans.load( QLocale( QLocale::Japanese ), QLatin1String( "tenkyu" ), QLatin1String( "_" ), QLatin1String( ":/translations" ), QLatin1String( ".qm" ) );
            }
        }

        QGuiApplication::installTranslator( &trans );
    }

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

