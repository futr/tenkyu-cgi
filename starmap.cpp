#include "starmap.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

StarMap::StarMap(QObject *parent) : QObject(parent)
{
    // Set default file name
    fnConstellationName      = "constellation_name_utf8.csv";
    fnHIP100                 = "hip_100.csv";
    fnHIPLiteA               = "hip_lite_a.csv";
    fnHIPLiteB               = "hip_lite_b.csv";
    fnConstellationLines     = "line.csv";
    fnConstellationPosition  = "position.csv";
    fnMessier = "messier.csv";
    fnNGC     = "ngc.csv";
}

bool StarMap::openDir(QString dirName)
{
    clear();

    QDir dir( dirName );

    if ( !dir.exists() ) {
        return false;
    }

    QFile consteNameFile( dir.filePath( fnConstellationName ) );
    QFile hip100File( dir.filePath( fnHIP100 ) );
    QFile hipAFile( dir.filePath( fnHIPLiteA ) );
    QFile hipBFile( dir.filePath( fnHIPLiteB ) );
    QFile consteLinesFile( dir.filePath( fnConstellationLines ) );
    QFile constePosFile( dir.filePath( fnConstellationPosition ) );
    QFile messierFile( dir.filePath( fnMessier ) );
    QFile NGCFile( dir.filePath( fnNGC ) );
    QTextStream s;

    // Read hip stars
    if ( !hipAFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) return false;
    if ( !hipBFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) return false;

    s.setDevice( &hipAFile );
    while ( !s.atEnd() ) {
        QStringList lineData = getLineData( s.readLine() );
        auto star = readStarDataLine( lineData );
        starHash.insert( star.id, star );
    }

    s.setDevice( &hipBFile );
    while ( !s.atEnd() ) {
        QStringList lineData = getLineData( s.readLine() );
        auto star = readStarDataLine( lineData );
        starHash.insert( star.id, star );
    }

    // Read famous 100 stars
    if ( !hip100File.open( QIODevice::ReadOnly | QIODevice::Text ) ) return false;

    s.setDevice( &hip100File );
    s.setCodec( "UTF-8" );
    while ( !s.atEnd() ) {
        QStringList lineData = getLineData( s.readLine() );
        auto star = readStarDataLine( lineData );
        starHash.insert( star.id, star );
        famousStarHash.insert( star.id, star );
    }

    // Create star list
    for ( auto &star : starHash ) {
        starList << star;
    }

    // Read constellations
    if ( !consteNameFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) return false;
    if ( !consteLinesFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) return false;
    if ( !constePosFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) return false;

    s.setDevice( &constePosFile );
    while ( !s.atEnd() ) {
        QStringList lineData = getLineData( s.readLine() );
        auto conste = readConstePosLine( lineData );
        consteHash.insert( conste.id, conste );
    }

    s.setDevice( &consteNameFile );
    s.setCodec( "UTF-8" );
    while ( !s.atEnd() ) {
        QStringList lineData = getLineData( s.readLine() );
        auto conste = readConsteNameLine( lineData );

        if ( consteHash.contains( conste.id ) ) {
            consteHash[conste.id].name = conste.name;
            consteHash[conste.id].nameEng = conste.nameEng;
        }
    }

    s.setDevice( &consteLinesFile );
    while ( !s.atEnd() ) {
        QStringList lineData = getLineData( s.readLine() );
        auto consteLine = readConsteLineLine( lineData );
        consteLineList << consteLine;
    }

    // Read messier
    if ( !messierFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) return false;

    s.setDevice( &messierFile );
    s.setCodec( "UTF-8" );
    while ( !s.atEnd() ) {
        QStringList lineData = getLineData( s.readLine() );
        auto messier = readMessierDataLine( lineData );
        messierList << messier;
    }

    return true;
}

void StarMap::clear()
{
    // Clear all data
    starList.clear();
    consteList.clear();
    starHash.clear();
    consteHash.clear();
    famousStarHash.clear();
    consteLineList.clear();
    messierList.clear();
}

StarData StarMap::readStarDataLine(QStringList line)
{
    StarData ret;

    if ( line.count() < 9 ) {
        ret.id = -1;
        return ret;
    }

    ret.id       = line[0].trimmed().toInt();
    double raH   = line[1].trimmed().toDouble();
    double raM   = line[2].trimmed().toDouble();
    double raS   = line[3].trimmed().toDouble();
    int deSign   = line[4].trimmed().toDouble();
    double deDeg = line[5].trimmed().toDouble();
    double deM   = line[6].trimmed().toDouble();
    double deS   = line[7].trimmed().toDouble();
    ret.mag      = line[8].trimmed().toDouble();
    ret.pos.setPos( raH, raM, raS, deSign, deDeg, deM, deS );

    if ( line.count() >= 18 ) {
        ret.name    = line[17].trimmed();
        ret.nameEng = line[16].trimmed();
    }

    ret.type = StarType::Star;

    return ret;
}

ConstellationData StarMap::readConstePosLine(QStringList line)
{
    // Read constellation positions
    ConstellationData ret;

    if ( line.count() < 4 ) {
        ret.id = -1;
        return ret;
    }

    ret.id       = line[0].trimmed().toInt();
    double raH   = line[1].trimmed().toDouble();
    double raM   = line[2].trimmed().toDouble();
    double deDeg = line[3].trimmed().toDouble();
    ret.pos.setPos( raH, raM, 0, 1, deDeg, 0, 0 );

    return ret;
}

ConstellationData StarMap::readConsteNameLine(QStringList line)
{
    // Read constellation name
    ConstellationData ret;

    if ( line.count() < 4 ) {
        ret.id = -1;
        return ret;
    }

    ret.id       = line[0].trimmed().toInt();
    ret.name     = line[3].trimmed();
    ret.nameEng  = line[2].trimmed();

    return ret;
}

CelestialLine StarMap::readConsteLineLine(QStringList line)
{
    // Read constellation line
    CelestialLine ret;

    if ( line.count() < 5 ) {
        ret.id = -1;
        return ret;
    }

    ret.id     = line[0].trimmed().toInt();
    double ra1 = line[1].trimmed().toDouble();
    double de1 = line[2].trimmed().toDouble();
    double ra2 = line[3].trimmed().toDouble();
    double de2 = line[4].trimmed().toDouble();

    ret.pos[0].setPos( ra1, de1 );
    ret.pos[1].setPos( ra2, de2 );

    return ret;
}

StarData StarMap::readMessierDataLine(QStringList line)
{
    StarData ret;

    if ( line.count() < 8 ) {
        ret.id = -1;
        return ret;
    }

    ret.id       = line[0].trimmed().toInt();
    double raH   = line[1].trimmed().toDouble();
    double raM   = line[2].trimmed().toDouble();
    double raMp  = line[3].trimmed().toDouble();
    int deSign   = line[4].trimmed().toDouble();
    double deDeg = line[5].trimmed().toDouble();
    double deM   = line[6].trimmed().toDouble();
    int type     = line[7].trimmed().toInt();
    QString name;

    if ( line.count() > 8 ) {
        name = line[8].trimmed();
    } else {
        name = QString( "M%1" ).arg( ret.id );
    }

    ret.pos.setPos( raH, raM + raMp / 10, 0, deSign, deDeg, deM, 0 );
    ret.name = name;
    ret.nameEng = name;

    switch ( type ) {
    case 0:
        ret.type = StarType::Others;
        break;
    case 1:
        ret.type = StarType::Galaxy;
        break;
    case 2:
        ret.type = StarType::Cluster;
        break;
    case 3:
        ret.type = StarType::Nebula;
        break;
    default:
        ret.type = StarType::Others;
    }

    return ret;
}

QStringList StarMap::getLineData(const QString &s)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    return s.split( ',', Qt::SkipEmptyParts );
#else
    return s.split( ',', QString::SkipEmptyParts );
#endif
}

CelestialPos::CelestialPos()
{
    ra = 0;
    de = 0;
}

CelestialPos::CelestialPos(double ra, double de)
{
    setPos( ra, de );
}

void CelestialPos::setPos(double raH, double raM, double raS, int sign, double deDeg, double deM, double deS)
{
    ra = raH * 15 + raM * 0.25 + raS * ( 0.25 / 60 );
    de = deDeg + deM / 60 + deS / 3600;

    if ( sign == 0 ) {
        de = -de;
    }
}

void CelestialPos::setPos(double ra, double de)
{
    this->ra = ra;
    this->de = de;
}

CelestialPos CelestialLine::getMidFromRA(double ra)
{
    // Get intermidiate pos
    CelestialPos ret;

    double grad = ( pos[1].de - pos[0].de ) / ( pos[1].ra - pos[0].ra );

    ret.ra = ra;
    ret.de = ( ra - pos[0].ra ) * grad + pos[0].de;

    return ret;
}

CelestialPos CelestialLine::getMidFromDE(double de)
{
    // Get intermidiate pos
    CelestialPos ret;

    double grad = ( pos[1].ra - pos[0].ra ) / ( pos[1].de - pos[0].de );

    ret.de = de;
    ret.ra = ( de - pos[0].de ) * grad + pos[0].ra;

    return ret;
}
