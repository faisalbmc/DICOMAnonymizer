#ifndef PATHDIALOG_H
#define PATHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

class PathDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PathDialog(QWidget *parent = 0);
    QLabel * sourceLabel;
    QLabel * destLabel;
    QTextEdit * sourceText;
    QTextEdit * destText;
    QPushButton * okPush;
    QPushButton * cancelButton;

    ~PathDialog()
    {
        delete sourceText;
        delete destText;
        delete okPush;
        delete cancelButton;
    }

signals:

public slots:

};

#endif // PATHDIALOG_H
