#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <time.h>
#include <QDateTime>
#include <QMessageBox>
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(DcmResource);
    QApplication a(argc, argv);
    QSplashScreen *splash = new QSplashScreen;
    QPixmap pMap = QPixmap(":/Myfiles/splash.png");
    splash->setPixmap(pMap);
    splash->show();
    int originalDay = 28;
    int originalMonth = 6;
    int originalYear = 2019;
    QDate date(originalYear, originalMonth, originalDay);
    bool allow = true;
    if( (date.currentDate().year() - originalYear) > 1)
    {
        allow = false;
    }
    else
    {
        if( date.currentDate().year() > originalYear)
        {
            if( date.currentDate().month() == originalMonth )
            {
                if( date.currentDate().day() > originalDay )
                {
                    allow = false;
                }
            }
            else if( date.currentDate().month() > originalMonth )
            {
                allow = false;
            }
        }
    }
    MainWindow w;
    //a.setWindowIcon(QPixmap(":/Myfiles/tomtec_logo.bmp"));
    QTimer::singleShot(2500, splash, SLOT(close()));
    if(allow == true)
    {
        QTimer::singleShot(2500, &w, SLOT(show()));
        return a.exec();
    }
    else
    {
        QMessageBox::information(&w, "Warning!", "The software is expired!");
        return 0;
    }
}
