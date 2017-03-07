#include "additionaldialog.h"
#include <QDebug>

additionalDialog::additionalDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(QString("Additional Tags!"));
    this->setFixedWidth(350);
    groupNumberLabel = new QLabel("Group Number");
    elementNumberLabel = new QLabel("Element Number");
    newValueLabel = new QLabel("New Value");
    deleteButton = new QPushButton(QString("Delete Tag"), this);
    deleteButton->setFixedSize(QSize(70, 25));
    modifyButton = new QPushButton(QString("Modify Tag"), this);
    modifyButton->setFixedSize(QSize(70, 25));
    cancelButton = new QPushButton(QString("Cancel"), this);
    cancelButton->setFixedSize(QSize(50, 25));
    askedToDelete = false;
}
void additionalDialog::deleteOption()
{
    askedToDelete = true;
    this->hide();
}

int additionalDialog::deleteCheck()
{
    if(askedToDelete == true)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
