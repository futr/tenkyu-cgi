#ifndef CELESTIALSPHEREPRINTER_H
#define CELESTIALSPHEREPRINTER_H

#include <QObject>
#include <QPainter>
#include <QPageSize>
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
public:
    QString getCreditText();
    int getStarCount() const;

signals:
    void processPage( int progress, int max );

public:
// private:
    bool startWritePDF( QString fileName, QIODevice *dev = nullptr );
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

private:
    StarMap starMap;
public:
    QPageSize pageSize = QPageSize( QPageSize::A4 );
    int printDPI       = 120;
    QPointF pageOffsetMm    = QPointF( 10, 10 );
    QPointF southOffsetMm   = QPointF( 5, 5 );
    QPointF segmentOffsetMm = QPointF( 0, 0 );
    double radius         = 128;
    double deRes          = 1;
    double forceHeight    = 130;
    double maxMagnitude   = 8;
    double obsLatitude    = 35;
    double starSize       = 6;
    double starSizeFactor = 1.30;
    int raSplit           = 6;
    int raRepeatCount     = 1;
    int deRepeatCount     = 1;
    bool inv            = true;
    bool isForcedHeight = false;
    bool funeMode       = true;
    bool printStarNames      = true;
    bool printConstellations = true;
    bool printMessiers       = true;
    bool printObsPointLine   = true;
    bool printNameUnderLine  = true;
    bool printCreditText     = true;
    bool useAlphabetText     = false;
    double starNamePoint     = 6.5;
    double messierNamePoint  = 6;
    double consteNamePoint   = 6.5;
    double infoStrPoint      = 7;
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
    QColor messierColor   = QColor( "#9400d3" );
    QColor obsPointColor  = QColor( Qt::darkGreen );
    QColor infoStrColor   = QColor( Qt::black );
    QColor starNameColor  = QColor( Qt::black );
    QColor underLineColor = QColor( Qt::gray );
    QColor gridColor      = QColor( Qt::gray );
    QColor frameColor     = QColor( Qt::black );
private:
    bool isNorth( int dePos );
};

#endif // CELESTIALSPHEREPRINTER_H
