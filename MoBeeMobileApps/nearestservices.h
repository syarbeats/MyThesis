#ifndef NEARESTSERVICES_H
#define NEARESTSERVICES_H

#include <QtGui/QWidget>
#include "ui_nearestservices.h"

class NearestServices : public QWidget
{
    Q_OBJECT

public slots:
    void showMainMenu();
    
public:
    NearestServices(QWidget *parent = 0);
    ~NearestServices();

private:
    Ui::NearestServicesClass ui;
};

#endif // NEARESTSERVICES_H
