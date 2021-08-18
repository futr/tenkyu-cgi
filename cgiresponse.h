#ifndef CGIRESPONSE_H
#define CGIRESPONSE_H

#include <QObject>
#include "celestialsphereprinter.h"

class CGIResponse : public QObject
{
    Q_OBJECT
public:
    explicit CGIResponse(QObject *parent = nullptr);

public slots:
    void doResponse();

signals:
private:
    CelestialSpherePrinter cp;
};

#endif // CGIRESPONSE_H
