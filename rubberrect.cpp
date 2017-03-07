#include "rubberrect.h"
#include <QDebug>

rubberrect::rubberrect(QWidget *parent) :
    QLabel(parent)
{
    moving = false;
    falsified = 0;
}

void rubberrect::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        mypoint = ev->pos();
        moving = true;
        tempBand = new customubberband(customubberband::Rectangle, this, QBrush(Qt::red));
    }
}

void rubberrect::mouseMoveEvent(QMouseEvent *ev)
{
    if((ev->buttons() & Qt::LeftButton)/*It's bitwise operation where the each mouse
                                         button is one of the bits. This means that even though the right button
                                         could be pressed along with the left button this would true, but the left must be
                                                 pressed for the whole code to yield true.*/&& moving)
    {
        if(mypoint.x() != ev->pos().x() || mypoint.y() != ev->pos().y())
        {
            tempBand->hide();
            tempBand->setGeometry(QRect(mypoint, ev->pos()).normalized());
            tempBand->show();
        }
    }
}

void rubberrect::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton && moving)
    {
        if(mypoint.x() != ev->pos().x() || mypoint.y() != ev->pos().y())
        {

            tempBand->hide();
            tempBand->setGeometry(QRect(mypoint, ev->pos()).normalized());
            tempBand->show();
            tempBand->exists = true;
            bandList.push_back(tempBand);
        }
        else if(mypoint.x() == ev->pos().x() && mypoint.y() == ev->pos().y())
        {
            for(int i=bandList.size(); i>0; i--)
            {
                if(mypoint.x() >= bandList.at(i-1)->geometry().left() && mypoint.x() <= bandList.at(i-1)->geometry().right() && mypoint.y() >= bandList.at(i-1)->geometry().top() && mypoint.y() <= bandList.at(i-1)->geometry().bottom())
                {
                    bandList.at(i-1)->hide();
                    bandList.at(i-1)->setGeometry(QRect(0,0,0,0));
                    bandList.at(i-1)->exists = false;
                    falsified++;
                }
            }
        }
        while(falsified>0)
        {
            for(int j = 0; j<bandList.size(); j++)
            {
                if(bandList.at(j)->exists == false)
                {
                    int a = bandList.size();
                    delete bandList.at(j);
                    bandList.remove(j);
                    bandList.squeeze();
                    j = j + a;
                    falsified--;
                }
            }
        }
        moving = false;
    }
    emit Mouse_Pressed();
}
