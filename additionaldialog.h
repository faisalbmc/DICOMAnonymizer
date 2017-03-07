#ifndef ADDITIONALDIALOG_H
#define ADDITIONALDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QString>
#include <QVector>

class additionalDialog : public QDialog
{
    Q_OBJECT
public:
    explicit additionalDialog(QWidget *parent = 0);
    QLabel * groupNumberLabel;
    QLabel * elementNumberLabel;
    QLabel * newValueLabel;
    QVector<QTextEdit *> groupNumber;
    QVector<QTextEdit *> elementNumber;
    QVector<QTextEdit *> newValue;
    QPushButton * modifyButton;
    QPushButton * deleteButton;
    QPushButton * cancelButton;
    bool askedToDelete;
    int deleteCheck();
    ~additionalDialog()
    {
        delete groupNumberLabel;
        delete elementNumberLabel;
        delete newValueLabel;
        delete modifyButton;
        delete deleteButton;
        delete cancelButton;
    }
signals:

public slots:
    void deleteOption();
};

#endif // ADDITIONALDIALOG_H
