#include "tableviewselector.h"

tableViewSelector::tableViewSelector(QWidget *parent) :
    QTableView(parent)
{
}

void tableViewSelector::mousePressEvent(QMouseEvent *ev)
{
    emit Mouse_Pressed();
}
