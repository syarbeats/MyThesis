#include "nearestservices.h"
#include <QTreeView>
#include <QStandardItemModel>

NearestServices::NearestServices(QWidget *parent)
    : QWidget(parent)
{
		ui.setupUi(this);
	
	    QStandardItemModel model( 5, 2 );
	    for( int r=0; r<5; r++ )
	    for( int c=0; c<2; c++)
	    {
	        QStandardItem *item =
	        new QStandardItem( QString("Row:%1, Column:%2").arg(r).arg(c) );
	        if( c == 0 )
	        for( int i=0; i<3; i++ )
	        item->appendRow( new QStandardItem( QString("Item %1").arg(i) ) );
	        model.setItem(r, c, item);
	    }

	    ui.treeServices->setModel( &model );
	    
}
void NearestServices::showMainMenu()
{
	close();
}

NearestServices::~NearestServices()
{

}
