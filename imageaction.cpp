#include "imageaction.h"

imageaction::imageaction(QWidget *parent) :
    QLabel(parent)
{
}

void imageaction::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    emit Mouse_Pressed();
}
