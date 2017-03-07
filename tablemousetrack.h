#ifndef TABLEMOUSETRACK_H
#define TABLEMOUSETRACK_H

#include <QTableView>

class tableMouseTrack : public QTableView
{
    Q_OBJECT
public:
    explicit tableMouseTrack(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
signals:
    void Mouse_right_button_pressed();
public slots:
    
};

#endif // TABLEMOUSETRACK_H
