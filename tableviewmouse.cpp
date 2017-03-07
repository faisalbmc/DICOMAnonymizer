#include "tableviewmouse.h"

tableViewMouse::tableViewMouse(QWidget *parent) :
    QTableWidget(parent)
{
}
void tableViewMouse::mousePressEvent(QMouseEvent *ev)
{
    emit Mouse_Pressed();
}

