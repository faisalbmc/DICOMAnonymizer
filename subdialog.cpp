#include "subdialog.h"
#include <QGridLayout>
subDialog::subDialog(QWidget *parent, QString PatientId, QString firstName, QString lastName, QString DOB, QString gender, QString studyDate, QString studyDescription, QString accessionNumber, QString instituteName, QString stationName,
                     QString instDeptName, QString operatorName/*, QString studyInstanceUID, QString seriesInstanceUID, QString studyID*/) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("De-identify study - Modify Patient Data");
    emptyLabel = new QLabel("");
    currentValue = new QLabel("Current Value");
    newValue = new QLabel("New Value");
    PatientID = new QLabel("Patient ID");
    PIDCurrent = new QTextEdit(PatientId,this);
    PIDNew = new QTextEdit(QString("ABCD1234"),this);
    FirstName = new QLabel("First Name");
    FirstNameCV = new QTextEdit(firstName, this);
    FirstNameNV = new QTextEdit(QString("John"),this);
    LastName = new QLabel("Last Name");
    LastNameCV = new QTextEdit(lastName, this);
    LastNameNV = new QTextEdit(QString("Doe"), this);
    DateOfBirth = new QLabel("Date of Birth");
    DateOfBirthCV = new QTextEdit(DOB, this);
    DateOfBirthNV = new QTextEdit(QString("19000101"), this);
    Gender = new QLabel("Gender");
    GenderCV = new QTextEdit(gender, this);
    GenderNV = new QTextEdit(QString("M"), this);
    StudyDate = new QLabel("Study Date");
    StudyDateCV = new QTextEdit(studyDate, this);
    StudyDateNV = new QTextEdit(QString("19000101"), this);
    StudyDescription = new QLabel("Study Description");
    StudyDescriptionCV = new QTextEdit(studyDescription, this);
    StudyDescriptionNV = new QTextEdit(QString("ECHO/STRESS"), this);
    AccessionNumber = new QLabel("Accession Number");
    AccessionNumberCV = new QTextEdit(accessionNumber, this);
    AccessionNumberNV = new QTextEdit(QString("951452"), this);
    InstituteName = new QLabel("Insitute Name");
    InstituteNameCV = new QTextEdit(instituteName, this);
    InstituteNameNV = new QTextEdit(QString("A Hospital"), this);
    StationName = new QLabel("Station Name");
    StationNameCV = new QTextEdit(stationName, this);
    StationNameNV = new QTextEdit(QString("A Station"), this);
    InstDeptName = new QLabel("Institutional Department Name");
    InstDeptNameCV = new QTextEdit(instDeptName, this);
    InstDeptNameNV = new QTextEdit(QString("A Department"), this);
    OperatorName = new QLabel("Operator Name");
    OperatorNameCV = new QTextEdit(operatorName, this);
    OperatorNameNV = new QTextEdit(QString("C operator"), this);
//    StudyInstanceUID = new QLabel("Study Instance UID");
//    StudyInstanceUIDCV = new QTextEdit(studyInstanceUID, this);
//    StudyInstanceUIDNV = new QTextEdit(QString("1.2.3.4.5.56.6.6.10"), this);
//    SeriesInstanceUID = new QLabel("Series Instance UID");
//    SeriesInstanceUIDCV = new QTextEdit(seriesInstanceUID, this);
//    SeriesInstanceUIDNV = new QTextEdit(QString("1.2.3.4.5.6.7.8.1.12.10"), this);
//    StudyID = new QLabel("Study ID");
//    StudyIDCV = new QTextEdit(studyID, this);
//    StudyIDNV = new QTextEdit(QString("123456"), this);
    CancelButton = new QPushButton(QString("Cancel"), this);
    OkButton = new QPushButton(QString("OK"), this);
    clearNewFields = new QPushButton(QString("Clear New Values"), this);
    additionalTags = new QPushButton(QString("Modify or delete additional tags"), this);
//    genNewInstancUID = new QPushButton(QString("New UID"), this);
    modifyMode = new QCheckBox(QString("Skip image modification"));
    takeDefectedImage = new QCheckBox(QString("Apply to all DICOM files with defective image"));
    PIDCurrent->setReadOnly(true);
    PIDCurrent->setFixedSize(QSize(200, 30));
    PIDNew->setFixedSize(QSize(200, 30));
    FirstNameCV->setReadOnly(true);
    FirstNameCV->setFixedSize(QSize(200, 30));
    FirstNameNV->setFixedSize(QSize(200, 30));
    LastNameCV->setReadOnly(true);
    LastNameCV->setFixedSize(QSize(200, 30));
    LastNameNV->setFixedSize(QSize(200, 30));
    DateOfBirthCV->setReadOnly(true);
    DateOfBirthCV->setFixedSize(QSize(200, 30));
    DateOfBirthNV->setFixedSize(QSize(200, 30));
    GenderCV->setReadOnly(true);
    GenderCV->setFixedSize(QSize(200, 30));
    GenderNV->setFixedSize(QSize(200, 30));
    StudyDateCV->setReadOnly(true);
    StudyDateCV->setFixedSize(QSize(200, 30));
    StudyDateNV->setFixedSize(QSize(200, 30));
    StudyDescriptionCV->setReadOnly(true);
    StudyDescriptionCV->setFixedSize(QSize(200, 30));
    StudyDescriptionNV->setFixedSize(QSize(200, 30));
    AccessionNumberCV->setReadOnly(true);
    AccessionNumberCV->setFixedSize(QSize(200, 30));
    AccessionNumberNV->setFixedSize(QSize(200, 30));
    InstituteNameCV->setReadOnly(true);
    InstituteNameCV->setFixedSize(QSize(200, 30));
    InstituteNameNV->setFixedSize(QSize(200, 30));
    StationNameCV->setReadOnly(true);
    StationNameCV->setFixedSize(QSize(200, 30));
    StationNameNV->setFixedSize(QSize(200, 30));
    InstDeptNameCV->setReadOnly(true);
    InstDeptNameCV->setFixedSize(QSize(200, 30));
    InstDeptNameNV->setFixedSize(QSize(200, 30));
    OperatorNameCV->setReadOnly(true);
    OperatorNameCV->setFixedSize(QSize(200, 30));
    OperatorNameNV->setFixedSize(QSize(200, 30));
//    StudyInstanceUIDCV->setReadOnly(true);
//    StudyInstanceUIDCV->setFixedSize(QSize(200, 30));
//    StudyInstanceUIDNV->setFixedSize(QSize(200, 30));
//    SeriesInstanceUIDCV->setReadOnly(true);
//    SeriesInstanceUIDCV->setFixedSize(QSize(200, 30));
//    SeriesInstanceUIDNV->setFixedSize(QSize(200, 30));
//    StudyIDCV->setReadOnly(true);
//    StudyIDCV->setFixedSize(QSize(200, 30));
//    StudyIDNV->setFixedSize(QSize(200, 30));
    clearNewFields->setFixedSize(QSize(100, 25));
    CancelButton->setFixedSize(QSize(50, 25));
    OkButton->setFixedSize(QSize(30, 25));
    additionalTags->setFixedSize(QSize(170, 25));
    modifyTask = false;
    deleteTask = false;
//    genNewInstancUID->setFixedSize(QSize(50, 20));
}

void subDialog::clearNewValues()
{
    PIDNew->clear();
    FirstNameNV->clear();
    LastNameNV->clear();
    DateOfBirthNV->clear();
    GenderNV->clear();
    StudyDateNV->clear();
    StudyDescriptionNV->clear();
    AccessionNumberNV->clear();
    InstituteNameNV->clear();
    StationNameNV->clear();
    InstDeptNameNV->clear();
    OperatorNameNV->clear();
}

void subDialog::tempBuild()
{
    tempIntake = new tempDialog(this);
    QGridLayout form(tempIntake);
    form.addWidget(tempIntake->numberLabel, 0, 0);
    form.addWidget(tempIntake->numberOfTags, 0, 1);
    tempIntake->numberOfTags->setTabChangesFocus(true);
    form.addWidget(tempIntake->okPush, 1, 1);
    form.addWidget(tempIntake->cancelButton, 1, 0);
    tempIntake->numberOfTags->setTabOrder(tempIntake->numberOfTags, tempIntake->cancelButton);
    tempIntake->cancelButton->setTabOrder(tempIntake->cancelButton, tempIntake->okPush);
    tempIntake->show();
    connect(tempIntake->okPush, SIGNAL(clicked()), tempIntake, SLOT(accept()));
    connect(tempIntake->cancelButton, SIGNAL(clicked()), tempIntake, SLOT(reject()));
    if(tempIntake->exec() == QDialog::Accepted)
    {
        int numberOfTextBox = tempIntake->numberOfTags->toPlainText().toInt(0, 10);
        miniDialog = new additionalDialog(this);
        QGridLayout form(miniDialog);
        form.addWidget(miniDialog->groupNumberLabel, 0, 0);
        form.addWidget(miniDialog->elementNumberLabel, 0, 1);
        form.addWidget(miniDialog->newValueLabel, 0, 2);
        form.addWidget(miniDialog->deleteButton, numberOfTextBox + 1, 0);
        for(int tagCounter = 0; tagCounter<numberOfTextBox; tagCounter++)
        {
            if(tagCounter == 0)
            {
                QTextEdit * numberEdit = new QTextEdit("");
                numberEdit->setFixedSize(QSize(70, 30));
                miniDialog->groupNumber.append(numberEdit);
                form.addWidget(miniDialog->groupNumber.at(tagCounter), tagCounter + 1, 0);
            }
            miniDialog->groupNumber.at(tagCounter)->setTabChangesFocus(true);
            QTextEdit * elementEdit = new QTextEdit("");
            elementEdit->setFixedSize(QSize(70, 30));
            miniDialog->elementNumber.append(elementEdit);
            form.addWidget(miniDialog->elementNumber.at(tagCounter), tagCounter + 1, 1);
            miniDialog->groupNumber.at(tagCounter)->setTabOrder(miniDialog->groupNumber.at(tagCounter), miniDialog->elementNumber.at(tagCounter));
            miniDialog->elementNumber.at(tagCounter)->setTabChangesFocus(true);
            QTextEdit * newValueEdit = new QTextEdit("");
            newValueEdit->setFixedSize(QSize(70, 30));
            miniDialog->newValue.append(newValueEdit);
            form.addWidget(miniDialog->newValue.at(tagCounter), tagCounter + 1, 2);
            miniDialog->elementNumber.at(tagCounter)->setTabOrder(miniDialog->elementNumber.at(tagCounter), miniDialog->newValue.at(tagCounter));
            miniDialog->newValue.at(tagCounter)->setTabChangesFocus(true);
            if(tagCounter<numberOfTextBox-1)
            {
                QTextEdit * numberEdit = new QTextEdit("");
                numberEdit->setFixedSize(QSize(70, 30));
                miniDialog->groupNumber.append(numberEdit);
                form.addWidget(miniDialog->groupNumber.at(tagCounter+1), tagCounter + 1 + 1, 0);
                miniDialog->newValue.at(tagCounter)->setTabOrder(miniDialog->newValue.at(tagCounter), miniDialog->groupNumber.at(tagCounter + 1));
            }
            else if(tagCounter == numberOfTextBox-1)
            {
                miniDialog->newValue.at(tagCounter)->setTabOrder(miniDialog->newValue.at(tagCounter), miniDialog->deleteButton);
            }
        }
        form.addWidget(miniDialog->modifyButton, numberOfTextBox + 1, 1);
        form.addWidget(miniDialog->cancelButton, numberOfTextBox + 1, 2);
        miniDialog->deleteButton->setTabOrder(miniDialog->deleteButton, miniDialog->modifyButton);
        miniDialog->modifyButton->setTabOrder(miniDialog->modifyButton, miniDialog->cancelButton);
        miniDialog->show();
        connect(miniDialog->deleteButton, SIGNAL(clicked()), miniDialog, SLOT(deleteOption()));
        connect(miniDialog->modifyButton, SIGNAL(clicked()), miniDialog, SLOT(accept()));
        connect(miniDialog->cancelButton, SIGNAL(clicked()), miniDialog, SLOT(reject()));
        if(miniDialog->exec() == QDialog::Accepted)
        {
            modifyTask = true;
            tagList.clear();
            for(int tagExtract = 0; tagExtract<numberOfTextBox; tagExtract++)
            {
                tagDetail tagTemp(miniDialog->groupNumber.at(tagExtract)->toPlainText(), miniDialog->elementNumber.at(tagExtract)->toPlainText(), miniDialog->newValue.at(tagExtract)->toPlainText());
                tagList.append(tagTemp);
            }
            miniDialog->close();
        }
        else if(miniDialog->deleteCheck() == 1)
        {
            deleteTask = true;
            tagList.clear();
            for(int tagExtract = 0; tagExtract<numberOfTextBox; tagExtract++)
            {
                tagDetail tagTemp(miniDialog->groupNumber.at(tagExtract)->toPlainText(), miniDialog->elementNumber.at(tagExtract)->toPlainText(), "");
                tagList.append(tagTemp);
            }
            miniDialog->close();
        }
    }
}
