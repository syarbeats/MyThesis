#ifndef CURRENTPOSITIONONTHEMAP_H
#define CURRENTPOSITIONONTHEMAP_H

#include <QtGui/QWidget>
#include "ui_currentpositiononthemap.h"

class CurrentPositionOnTheMap : public QWidget
{
    Q_OBJECT
    
private slots:
    void showMap();
    void backToMainMenu();
    

public:
    CurrentPositionOnTheMap(QWidget *parent = 0);
    ~CurrentPositionOnTheMap();
    

private:
    Ui::CurrentPositionOnTheMapClass ui;
};

#endif // CURRENTPOSITIONONTHEMAP_H
