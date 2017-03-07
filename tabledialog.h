#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QCheckBox>
#include <QPushButton>
#include <QStandardItemModel>

class tableDialog : public QDialog
{
    Q_OBJECT
public:
    explicit tableDialog(QWidget *parent = 0);
    QTableView * tagData;
    QCheckBox * modifyMode;
    QCheckBox * takeDefectedImage;
    QCheckBox * uidChangeIndicator;
    QPushButton * OkButton;
    QPushButton * CancelButton;
    QStandardItemModel * model;
    ~tableDialog()
    {
        delete tagData;
        delete modifyMode;
        delete takeDefectedImage;
        delete OkButton;
        delete CancelButton;
        delete uidChangeIndicator;
        delete model;
    }
signals:

public slots:

};

#endif // TABLEDIALOG_H
