#ifndef IMAGEACTION_H
#define IMAGEACTION_H

#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QEvent>

class imageaction : public QLabel
{
    Q_OBJECT
public:
    explicit imageaction(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
signals:
    void Mouse_Pressed();
public slots:
    
};

#endif // IMAGEACTION_H
