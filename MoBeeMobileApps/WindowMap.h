/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef WINDOWMAP_H
#define WINDOWMAP_H

#include <QtGui>

// QtMobility API headers
// Location
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
// Bearer
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

// QtMobility namespace
QTM_USE_NAMESPACE

class PictureItem;
class GoogleMapLoader;

class WindowMap: public QMainWindow
{
Q_OBJECT

public:
    WindowMap(QWidget *parent = 0);
    WindowMap(qreal longitude, qreal latitude);
    ~WindowMap();
    void resizeEvent(QResizeEvent *);
    void showMap();

private:
    void showSearchingGpsIcon(bool searching);
    void showMessage(QString msg);
    void createMenu();
    void startGps();
    void openDialog();

public slots:
    //void setupBearer();
    void positionUpdated(QGeoPositionInfo gpsPos);
    void mapImageLoaded(QPixmap);
    void updateMap();

private:
    // Graphics view fw
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    
    // Google Map API
    GoogleMapLoader* m_fetchmap;
    
   

    // Map picture
    QPixmap m_yourMapPixmap;
    QPointer<PictureItem> m_yourMapItem;

    // Animated message
    QPointer<PictureItem> m_messageItem;

    // Gps logo 
    PictureItem* m_gps;
    QPointer<QPropertyAnimation> m_gpsAnim;

    // for bearer management
    QNetworkSession* m_session;
    
    // Location data
    QPointer<QGeoPositionInfoSource> m_location;
    QGeoPositionInfo m_YougeoPosInfo;
    qreal m_friend_longitude;
    qreal m_friend_latitude;
    qreal m_current_latitude;
    qreal m_current_longitude;
};

#endif // WindowMap_H
