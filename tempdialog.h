#ifndef TEMPDIALOG_H
#define TEMPDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

class tempDialog : public QDialog
{
    Q_OBJECT
public:
    explicit tempDialog(QWidget *parent = 0);
    QLabel * numberLabel;
    QTextEdit * numberOfTags;
    QPushButton * okPush;
    QPushButton * cancelButton;
    ~tempDialog()
    {
        delete numberLabel;
        delete numberOfTags;
        delete okPush;
        delete cancelButton;
    }
signals:

public slots:
};

#endif // TEMPDIALOG_H
