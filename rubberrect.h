#ifndef RUBBERRECT_H
#define RUBBERRECT_H

#include "customubberband.h"
#include <QLabel>
#include <QRubberBand>
#include <QMouseEvent>
#include <QPoint>
#include <QVector>
#include <QTextEdit>

class rubberrect : public QLabel
{
    Q_OBJECT
public:
    explicit rubberrect(QWidget *parent = 0);
    QVector<customubberband*>bandList;
    customubberband * tempBand;
    QPoint mypoint;
    bool moving;
    int falsified;
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
signals:
    void Mouse_Pressed();
public slots:

};

#endif // RUBBERRECT_H
