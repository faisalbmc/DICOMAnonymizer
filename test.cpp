
#include "rubberrect.h"
#include "mainwindow.h"
#include <QDebug>


rubberrect::rubberrect(QWidget *parent) :
    QLabel(parent)
{
    moving = false;
    falsified = 0;
    numberOfBoxes = new QTextEdit(QString::number(0));
}

void rubberrect::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        mypoint = ev->pos();
        moving = true;
    }
}

void rubberrect::mouseMoveEvent(QMouseEvent *ev)
{
    if((ev->buttons() == Qt::LeftButton && moving)/*It's bitwise operation where the each mouse
                                                 button is one of the bits. This means that even though the right button
                                                 could be pressed along with the left button this would true, but the left must be
                                                         pressed for the whole code to yield true.*/&& moving)
    {
        if(mypoint.x() != ev->pos().x() || mypoint.y() != ev->pos().y())
        {
            customubberband * tempBand = new customubberband(customubberband::Rectangle, this, QBrush(Qt::red));
            bandList.push_back(tempBand);
            bandList.at(bandList.size()-1)->hide();
            bandList.at(bandList.size()-1)->setGeometry(QRect(mypoint, ev->pos()).normalized());
            bandList.at(bandList.size()-1)->show();
        }
    }
}

void rubberrect::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton && moving)
    {
        if(mypoint.x() != ev->pos().x() || mypoint.y() != ev->pos().y())
        {

            bandList.at(bandList.size()-1)->hide();
            bandList.at(bandList.size()-1)->setGeometry(QRect(mypoint, ev->pos()).normalized());
            bandList.at(bandList.size()-1)->show();
            bandList.at(bandList.size()-1)->exists = true;
            qDebug()<<"Number of boxes : "<<bandList.size()<<endl;
            numberOfBoxes->setText(QString::number(bandList.size()));
            numberOfBoxes->show();
            moving = false;
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
                    bandList.removeAt(j);
                    j = j + bandList.size();
                    falsified--;
                }
            }
        }
    }
}
