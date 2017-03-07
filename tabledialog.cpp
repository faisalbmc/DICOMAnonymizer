#include "tabledialog.h"

tableDialog::tableDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("De-identify study - Modify Patient Data");
    this->setFixedSize(680, 770);
    tagData = new QTableView(this);
    CancelButton = new QPushButton(QString("Cancel"), this);
    OkButton = new QPushButton(QString("OK"), this);
    modifyMode = new QCheckBox(QString("Skip image modification"));
    takeDefectedImage = new QCheckBox(QString("Apply to all DICOM files with defective image"));
    uidChangeIndicator = new QCheckBox(QString("Change Series instance UID, Study instance UID"));
    CancelButton->setFixedSize(QSize(50, 25));
    OkButton->setFixedSize(QSize(30, 25));
    tagData->setFixedSize(650, 650);
}
