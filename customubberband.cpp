#include "customubberband.h"


customubberband::customubberband(Shape s, QWidget *parent, QBrush brush) :
    QRubberBand(s, parent)
{
    exists = false;
    QPalette palette;
    palette.setBrush(QPalette::Foreground, brush);
    setPalette(palette);
}

void customubberband::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    QStyleOptionFocusRect option;
    option.initFrom(this);
    painter.setOpacity(0.5);
    painter.drawControl(QStyle::CE_FocusFrame, option);
}
