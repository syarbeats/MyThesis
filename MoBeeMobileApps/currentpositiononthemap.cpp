#include "currentpositiononthemap.h"
#include <QMessageBox>
#include "MoBEE.h"



CurrentPositionOnTheMap::CurrentPositionOnTheMap(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.showMapButton, SIGNAL(clicked()),this,SLOT(showMap()));
	connect(ui.backButton,SIGNAL(clicked()),this, SLOT(backToMainMenu()));
	
}

CurrentPositionOnTheMap::~CurrentPositionOnTheMap()
{

}

void CurrentPositionOnTheMap::backToMainMenu()
{
	this->close();	
}

void CurrentPositionOnTheMap::showMap()
{
	QString filename = "c:\\Data\\PositionOnTheMap.jpg";
	
	if(!filename.isEmpty())
	{
		QImage image(filename);
		if(image.isNull())
		{
			QMessageBox::information(this,tr("Position On The Map"),tr("Cannot load %1.").arg(filename));
			return;
		}
		
		ui.lblMap->setPixmap(QPixmap::fromImage(image));
		ui.lblMap->adjustSize();
	}
}

