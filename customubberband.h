#ifndef CUSTOMUBBERBAND_H
#define CUSTOMUBBERBAND_H

#include <QRubberBand>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include <QPalette>
#include <QBrush>

class customubberband : public QRubberBand
{
    Q_OBJECT
public:
    customubberband(Shape s, QWidget *parent, QBrush b);
    bool exists;
protected:
    virtual void paintEvent(QPaintEvent *);

signals:
    
public slots:
    
};

#endif // CUSTOMUBBERBAND_H
