#ifndef QTABLEMOUSETRACKER_H
#define QTABLEMOUSETRACKER_H

#include <QAbstractItemView>

class qtableMouseTracker : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit qtableMouseTracker(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
signals:
    void Mouse_right_button_pressed();
public slots:
    
};

#endif // QTABLEMOUSETRACKER_H
