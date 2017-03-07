#ifndef TABLEVIEWMOUSE_H
#define TABLEVIEWMOUSE_H

#include <QTableWidget>

class tableViewMouse : public QTableWidget
{
    Q_OBJECT
public:
    explicit tableViewMouse(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
signals:
    void Mouse_Pressed();

public slots:
    
};

#endif // TABLEVIEWMOUSE_H
