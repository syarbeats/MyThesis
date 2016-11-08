/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "WindowMap.h"
#include "PictureItem.h"
#include "GoogleMapLoader.h"


WindowMap::WindowMap(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("");
    
    m_friend_longitude = 0.0; // No friend position yet
    m_friend_latitude = 0.0;

    // Create QGraphicsScene and QGraphicsView
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    m_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setBackgroundBrush(QBrush(Qt::white));

    // Google Map fetcher
    m_fetchmap = new GoogleMapLoader(this);
    QObject::connect(m_fetchmap, SIGNAL(mapImageLoaded(QPixmap)),
    this, SLOT(mapImageLoaded(QPixmap)));
    m_yourMapPixmap = 0;

   
    // Create Satellite icon
    QPixmap pixmap(":/icons/Satellite.png");
    m_gps = new PictureItem(QSize(50, 50), pixmap, this);
    m_gps->setZValue(100);
    m_scene->addItem(m_gps);

    //createMenu();
    setCentralWidget(m_view);

    // Remove context menu from the all widgets
#ifdef Q_OS_SYMBIAN
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w = 0;
    foreach(w,widgets) 
        {
            w->setContextMenuPolicy(Qt::NoContextMenu);
        }
#endif    
    
   showMap();
}

WindowMap::~WindowMap()
{
    //m_location->stopUpdates();
    //m_session->close();
}

WindowMap::WindowMap(qreal longitude, qreal latitude)
{
    this->m_current_latitude = latitude;
    this->m_current_longitude = longitude;
}

void WindowMap::showMap()
{
	
	if(m_current_longitude!=0 && m_current_latitude!=0)
	{
		m_fetchmap->GoogleMapLoaderImage(size(), m_current_longitude, m_current_latitude, m_friend_longitude, m_friend_latitude);
	}
}

void WindowMap::startGps()
{
    // QGeoPositionInfoSource: Start GPS and listen position changes 
    showSearchingGpsIcon(true);
    if (!m_location) {
        m_location = QGeoPositionInfoSource::createDefaultSource(this);
        QObject::connect(m_location, SIGNAL(positionUpdated(QGeoPositionInfo)),
        this, SLOT(positionUpdated(QGeoPositionInfo)));
    }
    m_location->startUpdates();
}

void WindowMap::positionUpdated(QGeoPositionInfo gpsPos)
{
    // Take your GPS position only once and search map for that location
    if (gpsPos.isValid() && !m_YougeoPosInfo.isValid()) {
        // Get first position
        showMessage("GPS position get");
        m_YougeoPosInfo = gpsPos;
        showSearchingGpsIcon(false);

        // Check is there messages in the inbox

        // Search Map image
        m_fetchmap->GoogleMapLoaderImage(size(), m_YougeoPosInfo.coordinate().longitude(),
            m_YougeoPosInfo.coordinate().latitude(), m_friend_longitude, m_friend_latitude);
    }
    else if (gpsPos.isValid() && m_YougeoPosInfo.isValid()) {
        // Stop GPS, we have position
        showSearchingGpsIcon(false);
        m_location->stopUpdates();
    }
    else {
        showSearchingGpsIcon(true);
    }
}

void WindowMap::updateMap()
{
    // User wants to update his position to the map
    // Invalidate current position
    m_YougeoPosInfo.setCoordinate(QGeoCoordinate());

    // Remove old map picture
    if (m_yourMapItem) {
        m_yourMapItem->hide();
        m_scene->removeItem(m_yourMapItem);
        delete m_yourMapItem;
    }
    
    // Start GPS
    startGps();
}

void WindowMap::mapImageLoaded(QPixmap p)
{
    // Show loaded map image
    m_yourMapPixmap = p;
    m_yourMapItem = new PictureItem(m_yourMapPixmap.size(), m_yourMapPixmap, this);
    QPointF to = QPointF((rect().width() - m_yourMapItem->boundingRect().width()) / 2,
        (rect().height() - m_yourMapItem->boundingRect().height()) / 2);
    m_yourMapItem->setZValue(1);
    m_scene->addItem(m_yourMapItem);
    QPointF from = QPointF(to.x(), m_yourMapPixmap.height() * -1);
    m_yourMapItem->setPos(from);
    // Start animation for the image
    m_yourMapItem->animatePosition(from, to, 5000);
}

void WindowMap::resizeEvent(QResizeEvent *)
{
    // Update scene rect
    m_scene->setSceneRect(m_view->rect());

    // GPS item pos to top right corner
    m_gps->setPos(QPointF(m_scene->sceneRect().width() - m_gps->boundingRect().width() - 5, 5));

    // Get new map picture
    if (m_YougeoPosInfo.isValid()) {    
        if (m_yourMapItem) {
            m_yourMapItem->hide();
            m_scene->removeItem(m_yourMapItem);
            delete m_yourMapItem;
        }
        m_fetchmap->GoogleMapLoaderImage(size(), m_YougeoPosInfo.coordinate().longitude(),
            m_YougeoPosInfo.coordinate().latitude(), m_friend_longitude, m_friend_latitude);
    }
}

void WindowMap::showSearchingGpsIcon(bool searching)
{
    // Animate GPS logo or stop animation
    if (searching) {
        if (!m_gpsAnim)
            m_gpsAnim = m_gps->animateAngle(0, 360, true);
    }
    else {
        if (m_gpsAnim) {
            m_gps->stop(m_gpsAnim);
            m_gpsAnim = 0;
        }
    }
}

void WindowMap::createMenu()
{
    this->menuBar()->addAction("Update map", this, SLOT(updateMap()));
    this->menuBar()->addAction("Send your location", this, SLOT(sendYourLocation()));
    this->menuBar()->addAction("Ask friend location", this, SLOT(askFriendLocation()));
    this->menuBar()->addAction("Exit", qApp, SLOT(quit()));
}

void WindowMap::openDialog()
{
   
}

void WindowMap::showMessage(QString msg)
{
    // Show animated message to the user
    if (m_messageItem) {
        m_scene->removeItem(m_messageItem);
        delete m_messageItem;
        m_messageItem = 0;
    }

    // Create pixmap into message is drawed
    QPixmap pixmap(QSize(size().width() - 100, (size().width() - 100) / 3));
    QSize size = pixmap.size();

    QPainter painter(&pixmap);
    // Draw background
    painter.setBrush(Qt::white);
    painter.drawRect(pixmap.rect());
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::black);
    // Draw text
    painter.setFont(QFont("Helvetica", 6));
    QFontMetrics fm = painter.fontMetrics();
    QPoint c = QPoint((size.width() - fm.width(msg)) / 2, (size.height() - fm.height()) / 2);
    painter.drawText(pixmap.rect(), Qt::AlignCenter | Qt::TextWordWrap, msg);
    // Draw borders
    painter.drawRect(QRect(QPoint(2, 2), QSize(size.width() - 4, size.height() - 4)));
    painter.end();

    // Animate created pixmap
    m_messageItem = new PictureItem(pixmap.size(), pixmap, this);
    m_scene->addItem(m_messageItem);
    m_messageItem->setZValue(101);
    m_messageItem->setPos(QPointF((width() - size.width()) / 2, (height() - size.height()) / 2));
    m_messageItem->setHideWhenFinished();
    // Start animation
    m_messageItem->animateAngle(-90, 360);
}

