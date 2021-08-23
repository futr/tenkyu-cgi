#ifndef CELESTIALSPHEREPRINTER_H
#define CELESTIALSPHEREPRINTER_H

#include <QObject>
#include <QPainter>
#include <QPageSize>
#include <QDateTime>
#include "starmap.h"

class CelestialSpherePrinter : public QObject
{
    Q_OBJECT
public:
    explicit CelestialSpherePrinter(QObject *parent = nullptr);

public slots:
    QString generateFileName( bool useDate );
    bool openStarData( QString dirName );
    bool startPrinting( QString fileName );
    bool startWritePDF( QString fileName, QIODevice *dev = nullptr );
public:
    QString getCreditText();
    int getStarCount() const;

signals:
    void processPage( int progress, int max );

private:
    void paintStarFune( QPainter *painter, int raPos, int dePos, QPointF segOffsetMm, int dpi);
    void paintStarCap( QPainter *painter, int edgeDE, QPointF segOffsetMm, int dpi );
    double mmToPx( double mm, double dpi );
    QPointF mmToPx(QPointF p, double dpi );
    void drawStarToRawPx(QPainter *p, double x, double y, double magnitude );
    void drawEllipseToRawPx(QPainter *p, QPointF px, double w, double h, QPen pen );
    void drawCrossToRawPx( QPainter *p, QPointF px, double w, double h, QPen pen );

    QPointF radianPosOnFune( double centerRA, double ra, double de, bool invRA );
    QPointF mmPosOnFune(double dtheta, double phi );
    QPointF mmPosOnFuneToPrintPos(QPointF fposMm, int dpi , int dePos, QPointF segmOffsetMm);
    QPointF pxPosOnCap( CelestialPos pos, int edgeDE, bool invRA, QPointF segOffsetMm, int dpi );

    void drawStringOnFune(QPainter *p, QString str, QPen pen, QFont font, bool center, bool drawUnderLine, bool flipY, double autoRotateDec, QPointF strOffsetMm, int dePos, double dtheta, double phi, int dpi, QPointF segOffsetMm);
    void drawStringRawPxAling(QPainter *p, QString str, QPen pen, QFont font, int flags, bool drawBoundingRect, QPointF pos, double rotateDeg, QPointF offsetPx );
    void drawLineOnFune(QPainter *p, CelestialLine line, QPointF segOffsetMm, int raPos, int dePos, QPen pen, int dpi);
    void drawLineOnFune(QPainter *p, QPointF dtphi1, QPointF dtphi2, QPointF segOffsetMm, int dePos, QPen pen, int dpi);
    void drawLineOnCap(QPainter *p, CelestialLine line, QPointF segOffsetMm, int edgeDE, QPen pen, int dpi, bool invRA);

    void drawCreditText(QPainter *p, QPointF offsetMm, int dpi);
public:
    CelestialPos getObsPointZenith();
    QDateTime getLocalDateTime();
    double getGST( QDateTime dt );
    double getJulianDay( QDateTime dt );
    CelestialPos getPrecession(CelestialPos pos);
private:
    bool isNorth( int dePos );
private:
    StarMap starMap;
public:
    QPageSize pageSize = QPageSize( QPageSize::A4 );
    int printDPI       = 120;
    QPointF pageOffsetMm     = QPointF( 10, 10 );
    QPointF southOffsetMm    = QPointF( 0, 0 );
    QPointF segmentOffsetMm  = QPointF( 0, 0 );
    QPointF starNameOffsetOnFuneMm    = QPointF( 1.5, 0.1 );
    QPointF consteNameOffsetOnFuneMm  = QPointF( 0, 0 );
    QPointF messierNameOffsetOnFuneMm = QPointF( 1.0, 0 );
    double radius         = 128;
    double forceHeight    = 130;
    double maxMagnitude   = 8;
    double obsLatitude    = 35;
    double obsLongitude   = 135;
    int offsetHourFromUTC = 9;
    QDateTime obsLocalDateTime = QDateTime( QDate( 2021, 8, 20 ), QTime( 20, 0 ), Qt::TimeSpec::OffsetFromUTC, offsetHourFromUTC * 60 * 60 );
    //int obsLocalYear      = 2021;
    //int obsLocalMonth     = 8;
    //int obsLocalDay       = 20;
    //double obsLocalHour   = 20;
    //double obsLocalMin    = 0;
    double starSize       = 7;
    double starSizeFactor = 1.30;
    int raSplit           = 6;
    int raRepeatCount     = 1;
    int deRepeatCount     = 1;
    bool inv            = true;
    bool isForcedHeight = false;
    bool funeMode       = true;
    bool printStarNames       = true;
    bool printConsteName      = true;
    bool printMessierName     = true;
    bool printGrid            = true;
    bool printConstellations  = true;
    bool printMessiers        = true;
    bool printObsPointLine    = true;
    bool printNameUnderLine   = true;
    bool printCreditText      = true;
    bool printCoordinateText  = true;
    bool printSegmentInfoText = true;
    bool printObsPointZenith  = false;
    bool useAlphabetText      = false;
    double starNamePoint     = 7;
    double messierNamePoint  = 6.5;
    double consteNamePoint   = 7;
    double infoStrPoint      = 7.5;
    double gridLineWidth     = 1.5;
    double consteLineWidth   = 0.5;
    double messierLineWidth  = 0.5;
    double obsPointLineWidth = 0.5;
    double messierRadius     = 3.5;
    double capDecNorth       = 80;
    double capDecSouth       = -70;
    QColor bgColor        = QColor( Qt::white );
    QColor starColor      = QColor( Qt::blue );
    QColor consteColor    = QColor( Qt::red );
    QColor messierColor   = QColor( 0x94, 0x00, 0xd3 );
    QColor obsPointColor  = QColor( Qt::darkGreen );
    QColor infoStrColor   = QColor( Qt::black );
    QColor starNameColor  = QColor( Qt::black );
    QColor underLineColor = QColor( Qt::gray );
    QColor gridColor      = QColor( Qt::gray );
    QColor frameColor     = QColor( Qt::black );

};

#endif // CELESTIALSPHEREPRINTER_H
