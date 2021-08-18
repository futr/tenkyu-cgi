#ifndef STARMAP_H
#define STARMAP_H

#include <QObject>
#include <QHash>

class CelestialPos {
public:
    CelestialPos();
    CelestialPos( double ra, double de );
public:
    double ra;
    double de;
public:
    void setPos( double raH, double raM, double raS, int sign, double deDeg, double deM, double deS );
    void setPos( double ra, double de );
};

enum class StarType {
    Star,
    Galaxy,
    Cluster,
    Nebula,
    Others,
};

class StarData {
public:
    int id;
    CelestialPos pos;
    double mag;
    QString name;
    QString nameEng;
    StarType type;
};

class CelestialLine {
public:
    int id;
    CelestialPos pos[2];
    int starID[2];
public:
    CelestialPos getMidFromRA( double ra );
    CelestialPos getMidFromDE( double de );
};

class ConstellationData {
public:
    int id;
    CelestialPos pos;
    QString name;
    QString nameEng;
};

class StarMap : public QObject
{
    Q_OBJECT
public:
    explicit StarMap(QObject *parent = nullptr);

    bool openDir( QString dirName );
signals:

private:
    void clear();
    StarData readStarDataLine( QStringList line );
    ConstellationData readConstePosLine( QStringList line );
    ConstellationData readConsteNameLine( QStringList line );
    CelestialLine readConsteLineLine( QStringList line );
    StarData readMessierDataLine( QStringList line );
    QStringList getLineData( const QString &s );

public:
    QHash<int, StarData> starHash;
    QHash<int, StarData> famousStarHash;
    QHash<int, ConstellationData> consteHash;
    QList<StarData> starList;
    QList<ConstellationData> consteList;
    QList<CelestialLine> consteLineList;
    QList<StarData> messierList;

private:
    QString fnConstellationName;
    QString fnHIP100;
    QString fnHIPLiteA;
    QString fnHIPLiteB;
    QString fnConstellationLines;
    QString fnConstellationPosition;
    QString fnMessier;
    QString fnNGC;

    QString dirName;
};

#endif // STARMAP_H
