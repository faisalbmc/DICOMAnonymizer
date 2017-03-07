#ifndef SUBDIALOG_H
#define SUBDIALOG_H

#include <QGridLayout>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QString>
#include <QPushButton>
#include <QCheckBox>
#include "additionaldialog.h"
#include "tempdialog.h"


class tagDetail
{
public:
    QString groupNumber;
    QString elementNumber;
    QString value;
    tagDetail()
    {
        groupNumber = "";
        elementNumber = "";
        value = "";
    }
    tagDetail(QString a, QString b, QString c)
    {
        groupNumber = a;
        elementNumber = b;
        value = c;
    }
    ~tagDetail()
    {
    }
};

class subDialog : public QDialog
{
    Q_OBJECT
public:
    explicit subDialog(QWidget *parent = 0, QString PatientId = "", QString firstName = "", QString lastName = "", QString DOB = "", QString gender = "", QString studyDate = "", QString studyDescription = "",
    QString accessionNumber = "", QString instituteName = "", QString stationName = "", QString instDeptName = "", QString operatorName = ""/*, QString studyInstanceUID = "", QString seriesInstanceUID = "", QString studyID = ""*/);
    QLabel * emptyLabel;
    QLabel * currentValue;
    QLabel * newValue;
    QLabel * PatientID;
    QLabel * FirstName;
    QLabel * LastName;
    QLabel * DateOfBirth;
    QLabel * Gender;
    QLabel * StudyDate;
    QLabel * StudyDescription;
    QLabel * AccessionNumber;
    QLabel * InstituteName;
    QLabel * StationName;
    QLabel * InstDeptName;
    QLabel * OperatorName;
//    QLabel * StudyInstanceUID;
//    QLabel * SeriesInstanceUID;
//    QLabel * StudyID;
    QTextEdit * PIDCurrent;
    QTextEdit * PIDNew;
    QTextEdit * FirstNameCV;
    QTextEdit * FirstNameNV;
    QTextEdit * LastNameCV;
    QTextEdit * LastNameNV;
    QTextEdit * DateOfBirthCV;
    QTextEdit * DateOfBirthNV;
    QTextEdit * GenderCV;
    QTextEdit * GenderNV;
    QTextEdit * StudyDateCV;
    QTextEdit * StudyDateNV;
    QTextEdit * StudyDescriptionCV;
    QTextEdit * StudyDescriptionNV;
    QTextEdit * AccessionNumberCV;
    QTextEdit * InstituteNameCV;
    QTextEdit * StationNameCV;
    QTextEdit * InstDeptNameCV;
    QTextEdit * OperatorNameCV;
//    QTextEdit * StudyInstanceUIDCV;
//    QTextEdit * SeriesInstanceUIDCV;
//    QTextEdit * StudyIDCV;
    QTextEdit * AccessionNumberNV;
    QTextEdit * InstituteNameNV;
    QTextEdit * StationNameNV;
    QTextEdit * InstDeptNameNV;
    QTextEdit * OperatorNameNV;
//    QTextEdit * StudyInstanceUIDNV;
//    QTextEdit * SeriesInstanceUIDNV;
//    QTextEdit * StudyIDNV;
    QPushButton * clearNewFields;
    QPushButton * additionalTags;
    QPushButton * OkButton;
    QPushButton * CancelButton;
//    QPushButton * genNewInstancUID;
    QCheckBox * modifyMode;
    QCheckBox * takeDefectedImage;
    bool modifyTask;
    bool deleteTask;
    tempDialog * tempIntake;
    additionalDialog * miniDialog;
    QVector<tagDetail> tagList;
    ~subDialog()
    {
        delete emptyLabel;
        delete currentValue;
        delete newValue;
        delete PatientID;
        delete PIDCurrent;
        delete PIDNew;
        delete FirstName;
        delete FirstNameCV;
        delete FirstNameNV;
        delete LastName;
        delete LastNameCV;
        delete LastNameNV;
        delete DateOfBirth;
        delete DateOfBirthCV;
        delete DateOfBirthNV;
        delete Gender;
        delete GenderCV;
        delete GenderNV;
        delete StudyDate;
        delete StudyDateCV;
        delete StudyDateNV;
        delete StationName;
        delete StationNameCV;
        delete StationNameNV;
        delete StudyDescription;
        delete StudyDescriptionCV;
        delete StudyDescriptionNV;
//        delete StudyID;
//        delete StudyIDCV;
//        delete StudyIDNV;
//        delete SeriesInstanceUID;
//        delete SeriesInstanceUIDCV;
//        delete SeriesInstanceUIDNV;
//        delete StudyInstanceUID;
//        delete StudyInstanceUIDCV;
//        delete StudyInstanceUIDNV;
        delete AccessionNumber;
        delete AccessionNumberCV;
        delete AccessionNumberNV;
        delete InstituteName;
        delete InstituteNameCV;
        delete InstituteNameNV;
        delete InstDeptName;
        delete InstDeptNameCV;
        delete InstDeptNameNV;
        delete OperatorName;
        delete OperatorNameCV;
        delete OperatorNameNV;
        delete CancelButton;
        delete OkButton;
        delete clearNewFields;
        delete additionalTags;
//        delete genNewInstancUID;
        delete modifyMode;
        delete takeDefectedImage;
    }
signals:

public slots:
    void clearNewValues();
    void tempBuild();
};

#endif // SUBDIALOG_H
