/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef RECORDER_H
#define RECORDER_H

#include <experimental/qcamera.h>
#include <qmediarecorder.h>
#include <experimental/qstillimagecapture.h>
#include <qaudiocapturesource.h>
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
#include <QtGui>
#include <qsysteminfo.h>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include "XMLParser.h"
#include "currentpositiononthemap.h"
#include "nearestservices.h"
//#include <QSystemDeviceInfo>
//#include <QSystemNetworkInfo>
// Bearer
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#include "CurrentMap.h"
//#include "xqaccesspointmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MoBEE;
}
QT_END_NAMESPACE


#include <QMainWindow>
#include <QDir>

QTM_BEGIN_NAMESPACE
class QVideoWidget;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class MoBEE : public QMainWindow
{
    Q_OBJECT
public:
    MoBEE(QWidget *parent = 0);
    ~MoBEE();

private slots:
    void setCamera(const QByteArray &cameraDevice);
    void toggleCamera();
    void record();
    void pause();
    void stop();
    void takeImage();
    void settings();
    void displayErrorMessage();
    void updateCameraDevice(QAction*);
    void updateAudioDevice(QAction*);
    void updateCameraState(QCamera::State);
    void updateRecorderState(QMediaRecorder::State state);
    void updateRecordTime();
    void updateAudioDevices();
    void processCapturedImage(const QString& fname, const QImage& img);
    void positionUpdated(QGeoPositionInfo gpsPos);
    void setupBearer();
    void updateDataTransferProgress(qint64 readBytes,qint64 totalBytes);
    void closeConnection();
    void imageSaved (const QString & filename);
    //void getMarketingImage(QString strUrl);
    void getMarketingImageFinished(QNetworkReply *reply);
    void getCurrentObjectInformation(QString strUrl);
    void getCurrentObjectInformationFinished(QNetworkReply *reply);  
    void getInformationData();
    void getInformationRequest();
    void showGoogleMap();
    void sendImageAndXML();
    void handleNetworkData(QNetworkReply*);
    void exitFromApplication();
    void showNearestServices();
    
public :
    void searchCurrentLocation();
    QString getIMSI();
    QString getIMEI();
    QString getCellID();
    QString getOperatorName();
    QString getNetworkName();
    void createXMLClientApp();
    void sendXMLAndImageToServer(QString xmlfile, QString image);
    QString saveFileName(const QUrl &url);
    bool saveToDisk(const QString &filename, QIODevice *data);
    void GoogleMapLoaderImage(const QSize& size, qreal yourLong = 0, qreal yourLat = 0, qreal friendLong = 0, qreal friendLat = 0);
    void getMarketingImage(QString strUrl);    
    //void setupBearer();
    

private:
    Ui::MoBEE *ui;

    QDir outputDir;
    QCamera *camera;
    QStillImageCapture *imageCapture;
    QMediaRecorder* mediaRecorder;
    QAudioCaptureSource *audioSource;
    QVideoWidget *videoWidget;
    QString path;
    QString file;
    QString m_fullPath;
    QString fileJPG;
    QString imsi;
    QString imei;
    QString OperatorName;
    QString NetworkName;
    QString cellID;
    QPointer<QGeoPositionInfoSource> m_location;
    QGeoPositionInfo m_YougeoPosInfo;
    QString latitude;
    QString longitude;
    // for bearer management
    QNetworkSession* m_session;
    QSystemDeviceInfo *di;
    QSystemNetworkInfo *ni;
    XMLParser *xmlParser;
    QString datetime;
    QString xmlFilename;
    QFtp *connection;
    QNetworkAccessManager manager,informationRequest;
    QString server;
    QString serverPort;
    QTcpSocket *tcpSocket;
    QString serverURL;
    //CurrentMap *currentMap;
    CurrentPositionOnTheMap mapWindow;
    int nSend;
    bool isDefaultIapSet;
    QNetworkAccessManager m_manager;
    QUrl m_url;
    qreal rLatitude, rLongitude;
    NearestServices servicesWindow;
};

#endif
