/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef GOOGLEMAPLOADER_H
#define GOOGLEMAPLOADER_H

#include <QObject>
#include <QtNetwork>
#include <QPixmap>

// QtMobility API headers
// Bearer
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

// QtMobility namespace
QTM_USE_NAMESPACE

class GoogleMapLoader: public QObject
{
Q_OBJECT

public:
    GoogleMapLoader(QObject *parent = 0);
    ~GoogleMapLoader();

    void GoogleMapLoaderImage(const QSize& size, qreal yourLong = 0, qreal yourLat = 0, qreal friendLong =
        0, qreal friendLat = 0);

public slots:
    void handleNetworkData(QNetworkReply*);

signals:
    void mapImageLoaded(QPixmap);

private:
    QNetworkAccessManager m_manager;
    QUrl m_url;
    QNetworkSession* m_session;
};

#endif // GoogleMapLoader_H
