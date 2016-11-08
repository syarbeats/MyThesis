

#include "MoBEE.h"
#include "ui_MoBEE.h"
#include "settings.h"
#include "WindowMap.h"
#include "sym_iap_util.h"

#include <qmediaservice.h>
#include <qmediarecorder.h>
#include <experimental/qcamera.h>
#include <qvideowidget.h>
#include <QMessageBox>
#include <qmessagebox.h>
#include <QDateTime>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QtGui>

const QString
    GMAPS_STATICMAP_URL_TEMPLATE_You =
        "http://maps.google.com/maps/api/staticmap?center=%1,%2&zoom=14&size=%3x%4&maptype=mobile&markers=color:blue|label:Y|%1,%2&key=ABQIAAAAnfs7bKE82qgb3Zc2YyS-oBT2yXp_ZAY8_ufC3CFXhHIE1NvwkxSySz_REpPq-4WZA27OwgbtyR3VcA&sensor=false";

const QString
    GMAPS_STATICMAP_URL_TEMPLATE_You_and_Friend =
        "http://maps.google.com/maps/api/staticmap?center=%1,%2&size=%3x%4&maptype=mobile&markers=color:blue|label:Y|%1,%2&markers=color:red|label:F|%5,%6&key=ABQIAAAAnfs7bKE82qgb3Zc2YyS-oBT2yXp_ZAY8_ufC3CFXhHIE1NvwkxSySz_REpPq-4WZA27OwgbtyR3VcA&sensor=false";


MoBEE::MoBEE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MoBEE),
    camera(0),
    imageCapture(0),
    mediaRecorder(0),
    audioSource(0),
    videoWidget(0)
{
    ui->setupUi(this);

    outputDir = QDir::rootPath();
    
    datetime = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    
    //camera devices
    QByteArray cameraDevice;

    ui->actionCamera->setMenu(new QMenu(this));
    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
        QString description = deviceName+" "+camera->deviceDescription(deviceName);
        QAction *videoDeviceAction = new QAction(description, videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant(deviceName));
        if (cameraDevice.isEmpty()) {
            cameraDevice = deviceName;
            videoDeviceAction->setChecked(true);
        }
        ui->actionCamera->menu()->addAction(videoDeviceAction);
    }

    connect(videoDevicesGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateCameraDevice(QAction*)));

    ui->actionAudio->setMenu(new QMenu(this));

    setCamera(cameraDevice);
    
    searchCurrentLocation();
    
    di = new QSystemDeviceInfo(this);
    ni = new QSystemNetworkInfo(this);
    xmlParser = new XMLParser();
    
    
    
    this->imsi = getIMSI();
    this->imei = getIMEI();
    
    this->cellID = getCellID();
    ui->lblCellID->setText(cellID);
    ui->lblOperatorName->setText(getOperatorName());
    NetworkName = ui->lblOperatorName->text();
    xmlFilename = "c:\\Data\\"+imsi+".xml";
    this->server = "192.168.1.2";
    this->serverPort ="95000";
    tcpSocket = new QTcpSocket(this);
    nSend = 0;
    createXMLClientApp();
    
   // QTimer::singleShot(0, this, SLOT(setupBearer()));
    
    connect(ui->btnGoogleMap, SIGNAL(clicked()), this, SLOT(showGoogleMap()));
    connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(exitFromApplication()));
    connect(ui->servicesButton, SIGNAL(clicked()), this, SLOT(showNearestServices()));
    isDefaultIapSet = false;
    
    connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkData(QNetworkReply*)));
    //setupBearer();
    //this->statusBar()->showMinimized("Created by Syarif Hidayat",0);
}

void MoBEE::showNearestServices()
{
	servicesWindow.showFullScreen();
}

MoBEE::~MoBEE()
{
    delete mediaRecorder;
    delete videoWidget;
    delete camera;
    delete tcpSocket;
    delete di;
    delete ni;
    delete xmlParser;
    delete connection;
    delete m_session;
}

void MoBEE::exitFromApplication()
{		
	this->close();
}

void MoBEE::getMarketingImage(QString strUrl)
{
	QUrl url(strUrl);
	QNetworkRequest request(url);
	QNetworkReply *reply = manager.get(request);
	connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(getMarketingImageFinished(QNetworkReply*)));
}

void MoBEE::showGoogleMap()
{
	mapWindow.showFullScreen();
	
}

void MoBEE::GoogleMapLoaderImage(const QSize& size, qreal yourLong, qreal yourLat, qreal friendLong, qreal friendLat)
{
	 if(!isDefaultIapSet) 
     {
		  qt_SetDefaultIap();
		  isDefaultIapSet = true;
     }
	
	QString url;
    if (friendLong == 0 || friendLat == 0) {
        url = GMAPS_STATICMAP_URL_TEMPLATE_You .arg(QString::number(yourLat)) .arg(QString::number(
            yourLong)) .arg(QString::number(size.width())) .arg(QString::number(size.height()));
    }
    else {
        url = GMAPS_STATICMAP_URL_TEMPLATE_You_and_Friend .arg(QString::number(yourLat)) .arg(
            QString::number(yourLong)) .arg(QString::number(size.width())) .arg(QString::number(
            size.height())) .arg(QString::number(friendLat)) .arg(QString::number(friendLong));
    }

    m_url = QUrl(url);
    QNetworkRequest request;
    request.setUrl(m_url);
    m_manager.get(request);
}

void MoBEE::handleNetworkData(QNetworkReply *reply)
{
    // Get image from the reply
    QImage img;
    if (reply->error() == QNetworkReply::NoError) 
    {
        img.load(reply, 0);
        
        if (!img.isNull()) 
        {
			img.save("c:\\Data\\PositionOnTheMap.jpg","JPG");
			ui->btnGoogleMap->setEnabled(true);
        }
        
    }

    // Delete reply
    reply->deleteLater();

    // Emit image
    
}

QString MoBEE::saveFileName(const QUrl &url)
{
	    QString path = url.path();
	    QString basename = QFileInfo(path).fileName();

	    if (basename.isEmpty())
	        basename = "MarketingImage";

	    if (QFile::exists(basename)) {
	        // already exists, don't overwrite
	        int i = 0;
	        basename += '_';
	        while (QFile::exists(basename + QString::number(i)))
	            ++i;

	        basename += QString::number(i);
	    }

	    return basename;
}
    
bool MoBEE::saveToDisk(const QString &filename, QIODevice *data)
{
	    QFile file(filename);
	    if (!file.open(QIODevice::WriteOnly)) {
	        fprintf(stderr, "Could not open %s for writing: %s\n", qPrintable(filename), qPrintable(file.errorString()));
	        return false;
	    }

	    file.write(data->readAll());
	    file.close();

	    return true;
}

void MoBEE::getMarketingImageFinished(QNetworkReply *reply)
{
		ui->takeImageButton->setText("Please Wait is still download");
	    QUrl url = reply->url();
	    if (reply->error()) {
	        fprintf(stderr, "Download of %s failed: %s\n",
	                url.toEncoded().constData(),
	                qPrintable(reply->errorString()));
	    } else {
	        QString filename = "c:\\Data\\"+saveFileName(url);
	        if (saveToDisk(filename, reply))
	        {
					printf("Download of %s succeded (saved to %s)\n", url.toEncoded().constData(), qPrintable(filename));
					ui->takeImageButton->setText("Download was finished");
	        }
	    }
	    reply->deleteLater();
	    
}

void MoBEE::getInformationRequest()
{
	   
	    //QMessageBox msgBox;
	   /// nSend++;
	    //msgBox.setText("Connect ke "+nSend);
		//msgBox.exec();
		
		tcpSocket->connectToHost(this->server, this->serverPort.toInt());
	    //connect(tcpSocket, SIGNAL(connected()), this, SLOT(getInformationData()));
		//connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getInformationData()));
		if(tcpSocket->waitForConnected(10*1000))
		{
				QString script="sift <"+imsi+".pgm> "+imsi+".key";
			    tcpSocket->write(script.toLocal8Bit());		
			    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(getInformationData()));
		}
	    
}

void MoBEE::getInformationData()
{
	    
		 serverURL = tcpSocket->readAll();
		 ui->txtInformation->append(serverURL);
		 tcpSocket->disconnectFromHost();
		 tcpSocket->waitForDisconnected();
		 ui->progressBar->setValue(0);
		 this->getMarketingImage(serverURL);
		 //m_session->close();
	
	    /*if(tcpSocket->waitForReadyRead(50*1000))
	    {
	            serverURL = tcpSocket->readAll();
	    		//ui.InformatioinTextEdit->setText(reply);
	            ui->txtInformation->append(serverURL);
	    		
	    }
	    else
	    {
	    		QMessageBox msgBox;
	            msgBox.setText("Sift result could not be read");
	            msgBox.exec();
	    }
	    
	    tcpSocket->disconnectFromHost();
	    tcpSocket->waitForDisconnected();
	    ui->uploadButton->setEnabled(false);
	    //serverURL = tcpSocket->readAll();
	    //tcpSocket->disconnectFromHost();
		//connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(getCurrentObjectInformation(serverURL)));
		*/	
}

void MoBEE::getCurrentObjectInformation(QString strUrl)
{
		QUrl url(strUrl);
		QNetworkRequest request(url);
		QNetworkReply *reply = informationRequest.get(request);
		connect(&informationRequest, SIGNAL(finished(QNetworkReply*)), SLOT(getCurrentObjectInformationFinished(QNetworkReply*)));
}
void MoBEE::getCurrentObjectInformationFinished(QNetworkReply *reply)
{
			QUrl url = reply->url();
		    if (reply->error()) {
		        fprintf(stderr, "Download of %s failed: %s\n",
		                url.toEncoded().constData(),
		                qPrintable(reply->errorString()));
		    } else {
		        /*QString filename = "c:\\Data\\"+saveFileName(url);
		        if (saveToDisk(filename, reply))
		            printf("Download of %s succeded (saved to %s)\n",
		                   url.toEncoded().constData(), qPrintable(filename));
		         */
		        
		         ui->txtInformation->append(reply->readAll());
		    
		    }
		    reply->deleteLater();
}

void MoBEE::sendImageAndXML()
{
	//this->sendXMLAndImageToServer(this->xmlFilename,"c:\\Data\\Images\\"+this->file);
}

void MoBEE::sendXMLAndImageToServer(QString xmlfile, QString image)
{
	    if(!isDefaultIapSet) 
	    {
	        qt_SetDefaultIap();
	        isDefaultIapSet = true;
	    }
	
	    QFile transferedImage(image);
	    QFile transferedXML(xmlfile);
	    

	    if (!transferedImage.open(QIODevice::ReadOnly))
	    {

	         QMessageBox msgBox;
	         msgBox.setText("Image File Not Found");
	         msgBox.exec();

	    }

	    if (!transferedXML.open(QIODevice::ReadOnly))
	    {

	         QMessageBox msgBox;
	         msgBox.setText("XML File Not Found");
	         msgBox.exec();

	    }

	    QByteArray arrayImage = transferedImage.readAll();
	    QByteArray arrayXML = transferedXML.readAll();

	    connection = new QFtp();
	    connect(connection, SIGNAL(dataTransferProgress(qint64,qint64)),this, SLOT(updateDataTransferProgress(qint64,qint64)));
	    
	    connect(connection, SIGNAL(done(bool)), this, SLOT(closeConnection()));
	    connect(connection, SIGNAL(done(bool)), this, SLOT(getInformationRequest()));
	    
	    
	    connection->connectToHost("192.168.1.2");
	    connection->login("syarif", "syarif");
	    connection->put(arrayImage, imsi+".jpg", QFtp::Binary);
	    connection->put(arrayXML, imsi+".xml", QFtp::Binary);
}

void MoBEE::updateDataTransferProgress(qint64 readBytes,qint64 totalBytes)
{
	ui->progressBar->setMaximum(totalBytes);
	ui->progressBar->setValue(readBytes);
}

void MoBEE::closeConnection()
{
	this->connection->close();
	
}

void MoBEE::createXMLClientApp()
{
  
	xmlParser->imsi.insertMulti("imsi",this->imsi);
	xmlParser->OperatorName.insertMulti("OperatorName",ui->lblOperatorName->text());
	xmlParser->NetworkName.insertMulti("NetworkName",NetworkName);
	xmlParser->latitude.insertMulti("latitude",ui->lblLatitude->text());
	xmlParser->longitude.insertMulti("longitude",ui->lblLongitude->text());
	xmlParser->cellid.insertMulti("cellid",this->cellID);
	xmlParser->datetime.insertMulti("datetime",datetime);
	xmlParser->CreateXMLFile(this->imei,this->xmlFilename);                        
        			
}

QString MoBEE::getCellID()
{
	return QString::number(ni->cellId());
}

QString MoBEE::getIMSI()
{
		    
	return di->imsi();
}

QString MoBEE::getIMEI()
{
	return di->imei();
}

QString MoBEE::getOperatorName()
{
	
	return ni->networkName((QSystemNetworkInfo::NetworkMode)1);
}

QString MoBEE::getNetworkName()
{
	return ni->networkName((QSystemNetworkInfo::NetworkMode)1);
}

void MoBEE::searchCurrentLocation()
{
	
	   if (!m_location) {
	        m_location = QGeoPositionInfoSource::createDefaultSource(this);
	        QObject::connect(m_location, SIGNAL(positionUpdated(QGeoPositionInfo)),
	        this, SLOT(positionUpdated(QGeoPositionInfo)));
	    }
	    m_location->startUpdates();
}

void MoBEE::positionUpdated(QGeoPositionInfo gpsPos)
{
    
    if (gpsPos.isValid()) {
        //showMessage("GPS position get");
        //showSearchingGpsIcon(false);
        ui->lblLatitude->setText(QString::number(gpsPos.coordinate().latitude()));
        ui->lblLongitude->setText(QString::number(gpsPos.coordinate().longitude()));
        rLongitude = gpsPos.coordinate().longitude();
        rLatitude  = gpsPos.coordinate().latitude();
        
        if(ui->lblLatitude->text()!="0" && ui->lblLongitude->text()!="0")
        {
				
				QSize size;
				size.setWidth(640);
				size.setHeight(480);
				GoogleMapLoaderImage(size,rLongitude,rLatitude,0,0);
        }
   
    }
    else if (gpsPos.isValid() && m_YougeoPosInfo.isValid()) {
        //showSearchingGpsIcon(false);
        m_location->stopUpdates();
    }
    else {
        //showSearchingGpsIcon(true);
    }
}

void MoBEE::setupBearer()
{
    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities() & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || !canStartIAP) {
        QMessageBox::information(this, "MoBEE", "Available Access Points not found");
        return;
    }
    m_session = new QNetworkSession(cfg);
    m_session->open();
    m_session->waitForOpened();
}

void MoBEE::setCamera(const QByteArray &cameraDevice)
{
    delete imageCapture;
    delete mediaRecorder;
    delete videoWidget;
    delete camera;

    if (cameraDevice.isEmpty())
        camera = new QCamera;
    else
        camera = new QCamera(cameraDevice);

    ui->startCameraButton->click();
    
    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));

    mediaRecorder = new QMediaRecorder(camera);
    connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)));

    imageCapture = new QStillImageCapture(camera);

    audioSource = new QAudioCaptureSource(camera);
    connect(audioSource, SIGNAL(availableAudioInputsChanged()), SLOT(updateAudioDevices()));

    mediaRecorder->setOutputLocation(QUrl("test.mkv"));

    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    camera->setMetaData(QtMedia::Title, QVariant(QLatin1String("Test Title")));

    videoWidget = new QVideoWidget;
    videoWidget->setMediaObject(camera);
    ui->stackedWidget->addWidget(videoWidget);

    updateCameraState(camera->state());
    updateRecorderState(mediaRecorder->state());
    updateAudioDevices();

    //connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), ui->imageCaptureBox, SLOT(setEnabled(bool)));
    //connect(imageCapture, SIGNAL(imageCaptured(QString,QImage)), this, SLOT(processCapturedImage(QString,QImage)));
    connect(imageCapture, SIGNAL(imageSaved (const QString &)), this, SLOT(imageSaved (const QString &)));

}

void MoBEE::updateAudioDevices()
{
    ui->actionAudio->menu()->clear();
    QActionGroup *audioDevicesGroup = new QActionGroup(this);
    audioDevicesGroup->setExclusive(true);

    if (audioSource->isAvailable()) {
        QList<QString> devices = audioSource->audioInputs();
        for (int i=0; i<devices.size(); i++) {
            QString description = audioSource->audioDescription(devices.at(i));
            QAction *audioDeviceAction = new QAction(devices.at(i)+" "+description, audioDevicesGroup);
            audioDeviceAction->setData(devices.at(i));
            audioDeviceAction->setCheckable(true);

            ui->actionAudio->menu()->addAction(audioDeviceAction);

            if (devices.at(i) == audioSource->activeAudioInput())
                audioDeviceAction->setChecked(true);
        }
    } else {
        qWarning() << "No audio device for camera service available";
    }

    connect(audioDevicesGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateAudioDevice(QAction*)));
}

void MoBEE::updateRecordTime()
{
    QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration()/1000);
    //ui->statusbar->showMessage(str);
}

void MoBEE::processCapturedImage(const QString& fname, const QImage& img)
{
    //qDebug() << "image captured:" << fname;
	
	
	
}


void MoBEE::imageSaved (const QString &filename)
{
	
	
		sendXMLAndImageToServer(xmlFilename,"c:\\Data\\Images\\"+this->file);
		
}


void MoBEE::settings()
{
    Settings settingsDialog(mediaRecorder);

    settingsDialog.setAudioSettings(mediaRecorder->audioSettings());
    settingsDialog.setVideoSettings(mediaRecorder->videoSettings());
    settingsDialog.setFormat(mediaRecorder->containerMimeType());

    if (settingsDialog.exec()) {
        mediaRecorder->setEncodingSettings(
                settingsDialog.audioSettings(),
                settingsDialog.videoSettings(),
                settingsDialog.format());
    }
}

void MoBEE::record()
{
    mediaRecorder->record();
    updateRecordTime();
}

void MoBEE::pause()
{
    mediaRecorder->pause();
}

void MoBEE::stop()
{
    mediaRecorder->stop();
}

void MoBEE::takeImage()
{
    //path = "c:\\Data\\Images\\";
    //file = this->imsi + "_" +QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".jpg";
    file = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".jpg";
    //m_fullPath = path + file;
    //fileJPG = imsi+".jpg";
    
    //imageCapture->capture(m_fullPath);    
    nSend=0;
    imageCapture->capture(file);
                                                    
}

void MoBEE::toggleCamera()
{
    if (camera->state() == QCamera::ActiveState)
        camera->stop();
    else
        camera->start();
}

void MoBEE::updateCameraState(QCamera::State state)
{
    if (state == QCamera::ActiveState) {
        ui->actionCamera->setEnabled(false);
        ui->actionAudio->setEnabled(false);
        ui->actionSettings->setEnabled(true);

        ui->startCameraButton->setText(tr("Stop Camera"));
        ui->startCameraButton->setChecked(true);
        
    } else {
        ui->actionCamera->setEnabled(true);
        ui->actionAudio->setEnabled(true);
        ui->actionSettings->setEnabled(true);

        ui->startCameraButton->setText(tr("Start Camera"));
        ui->startCameraButton->setChecked(false);
        
    }

    if (camera->isAvailable()) {
        ui->startCameraButton->setEnabled(true);
    } else {
        ui->startCameraButton->setEnabled(false);
        ui->startCameraButton->setText(tr("Camera is not available"));
    }
}

void MoBEE::updateRecorderState(QMediaRecorder::State state)
{
    switch (state) {
    case QMediaRecorder::StoppedState:
        
        break;
    case QMediaRecorder::PausedState:
       
        break;
    case QMediaRecorder::RecordingState:
        
        break;
    }
}


void MoBEE::displayErrorMessage()
{
    QMessageBox::warning(this, "Capture error", mediaRecorder->errorString());
}

void MoBEE::updateCameraDevice(QAction *action)
{
    setCamera(action->data().toByteArray());
}

void MoBEE::updateAudioDevice(QAction *action)
{
    audioSource->setAudioInput(action->data().toString());
}

