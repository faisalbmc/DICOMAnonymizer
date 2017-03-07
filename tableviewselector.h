#ifndef TABLEVIEWSELECTOR_H
#define TABLEVIEWSELECTOR_H

#include <QTableView>

class tableViewSelector : public QTableView
{
    Q_OBJECT
public:
    explicit tableViewSelector(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
signals:
    void Mouse_Pressed();
    
public slots:
    
};

#endif // TABLEVIEWSELECTOR_H
