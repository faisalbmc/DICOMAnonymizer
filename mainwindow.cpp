// The code fo DCMIncognito
// Written by Faisal Ibne Mozher
// Special mention of DCMTK library

// DCMTK libraries
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpeg/djencode.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmjpeg/djrploss.h"
#include "dcmtk/dcmjpeg/djrplol.h"
#include "dcmtk/dcmdata/dcistrmf.h"
#include "dcmtk/dcmdata/dcostrmf.h"
#include "mdfdsman.h"


// Regular C/C++ libraries
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>

// Self made class header files
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageaction.h"
#include "rubberrect.h"
#include "customubberband.h"
//#include "subdialog.h"
//#include "tempdialog.h"
#include "tabledialog.h"

// QT libraries
#include <QFileDialog>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QRectF>
#include <QRubberBand>
#include <QListView>
#include <QTreeView>
#include <QMessageBox>
#include <QImage>
#include <QFormLayout>
#include <QShortcut>
#include <QProgressDialog>
#include <QProgressBar>
#include <QFileInfo>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), directory("/")
{
    ui->setupUi(this);
    setWindowTitle("DCMIncognito - Dicom image anonymizer");
    //this->setFixedSize(QSize(1351, 810));
    inModifyFunc = false;
    controlButtonCounter = 0;
    fetch = false;
    tagModified = false;
    errorLogOpen = false;
    overrideCursor = false;
    emergeBreak = false;
    deleteImage = false;
    counter = 0;
    tableCounter = 0;
    errorCounter = 0;
    firstTime = true;
    destinationSelected = false;
    bigImageShown = false;
    tagModelStarted = false;
    UIDChanges = false;
    ui->listOfImages->viewport()->installEventFilter(this);
    ui->listOfImages->installEventFilter(this);
    ui->selectedFiles->installEventFilter(this);
    ui->finishedList->show();
    ui->listOfdefectedPixelData->show();
    ui->WrongList->show();
    ui->closeBigImage->hide();
    ui->bigImageLabel->hide();
    ui->modifyTheImage->hide();
    ui->progressBar->hide();
    ui->progressBar->setValue(0);
    ui->label->hide();
    ui->errorClose->hide();
    ui->clearError->hide();
    ui->errorEdit->hide();
    ui->refresh->hide();
    ui->dontShowError->hide();
    ui->listOfImages->setFixedWidth(280);
    ui->listOfdefectedPixelData->setFixedWidth(280);
    ui->bigImageLabel->setFixedSize(811, 511);
    ui->boxes->setFixedSize(30, 25);
    ui->boxes->hide();
    ui->boxesLabel->hide();
    //ui->nonUIoption->hide();
    taskIndicator = 0;
    QRect LhorzRect = ui->horizontalSpacer_9->geometry();
    ui->horizontalSpacer_9->setGeometry(QRect(LhorzRect.left(), LhorzRect.top(), LhorzRect.width()+30, LhorzRect.height()));
    ui->listOfImages->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->selectFile, SIGNAL(clicked()), this, SLOT(showTheSelectedList()));
    connect(ui->label, SIGNAL(Mouse_Pressed()), this, SLOT(showBigImage()));
    connect(ui->closeBigImage, SIGNAL(clicked()), this, SLOT(closeBigImage()));
    connect(ui->modifyTheImage, SIGNAL(clicked()), this, SLOT(modifyingTheImage()));
    connect(ui->destSelect, SIGNAL(clicked()), this, SLOT(destinationDirectory()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearFiles()));
    connect(ui->listOfImages, SIGNAL(pressed(const QModelIndex&)), this, SLOT(tableClicked(const QModelIndex&)));
    connect(ui->selectAll, SIGNAL(clicked()), this, SLOT(selectAllImage()));
    connect(ui->ErrorButton, SIGNAL(clicked()), this, SLOT(showError()));
    connect(ui->errorClose, SIGNAL(clicked()), this, SLOT(closeError()));
    connect(ui->tagChanger, SIGNAL(clicked()), this, SLOT(tagChanging()));
    connect(ui->refresh, SIGNAL(clicked()), this, SLOT(refresher()));
    connect(ui->selectedFiles, SIGNAL(pressed(const QModelIndex&)), this, SLOT(imageChange(const QModelIndex&)));
    connect(ui->clearError, SIGNAL(clicked()), this, SLOT(clearErrorLog()));
    connect(ui->nonUIoption, SIGNAL(clicked()), this, SLOT(nonUIoperation()));
    connect(ui->bigImageLabel, SIGNAL(Mouse_Pressed()), this, SLOT(checkNOofBoxes()));
    //connect(ui->UidTest, SIGNAL(clicked()), this, SLOT(IDListFillup()));
    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::SelectAll), ui->listOfImages);
    connect(shortcut, SIGNAL(activated()), this, SLOT(selectAllImage()));
}

void MainWindow::checkNOofBoxes()
{
    ui->boxes->setText(QString::number(ui->bigImageLabel->bandList.size()));
    ui->boxes->show();
}

void MainWindow::nonUIoperation()
{
    pathDialog = new PathDialog(this);
    QGridLayout form(pathDialog);
    form.addWidget(pathDialog->sourceLabel, 0, 0, 1, 1, Qt::AlignLeft);
    form.addWidget(pathDialog->sourceText, 1, 0, 1, 1, Qt::AlignLeft);
    form.addWidget(pathDialog->destLabel, 2, 0, 1, 1, Qt::AlignLeft);
    form.addWidget(pathDialog->destText, 3, 0, 1, 1, Qt::AlignLeft);
    form.addWidget(pathDialog->cancelButton, 4, 0, 1, 1, Qt::AlignLeft);
    form.addWidget(pathDialog->okPush, 4, 1, 1, 1, Qt::AlignLeft);
    connect(pathDialog->okPush, SIGNAL(clicked()), pathDialog, SLOT(accept()));
    connect(pathDialog->cancelButton, SIGNAL(clicked()), pathDialog, SLOT(reject()));
    int execStatus = pathDialog->exec();
    if(execStatus == QDialog::Accepted)
    {
        QString sourcepath = pathDialog->sourceText->toPlainText();
        QString destpath = pathDialog->destText->toPlainText();
        QDir sourceDir(sourcepath);
        QDir destDir(destpath);
        bool sourceCheck = sourceDir.exists();
        bool destCheck = destDir.exists();
        if(sourceDir.isRoot())
        {
            sourceCheck = false;
        }
        if(destDir.isRoot())
        {
            destCheck = false;
        }
        if(sourceCheck == false && destCheck == true)
        {
            QMessageBox::information(this, "Message!", "Your source path is invalid or a root directory(root directory not allowed)!");
        }
        else if(destCheck == false && sourceCheck == true)
        {
            QMessageBox::information(this, "Message!", "Your destination path is invalid or a root directory(root directory not allowed)!!");
        }
        else if(destCheck == false && sourceCheck == false)
        {
            QMessageBox::information(this, "Message!", "Your source and destination path are both invalid or are root directories(root directory not allowed)!!");
        }
        else
        {
            QString tagPath = sourceDir.absolutePath();
            tagPath.append("/tagrules.txt");
            QFile tagfile(tagPath);

            QString vendorPath = sourceDir.absolutePath();
            vendorPath.append("/vendorrules.txt");
            QFile vendorfile(vendorPath);

            if(!tagfile.open(QIODevice::ReadOnly) && vendorfile.open(QIODevice::ReadOnly))
            {
                QMessageBox::information(this, "Problem with rules file for tags!", tagfile.errorString());
            }
            else if(tagfile.open(QIODevice::ReadOnly) && !vendorfile.open(QIODevice::ReadOnly))
            {
                QMessageBox::information(this, "Problem with rules file for vendors!", vendorfile.errorString());
            }
            else if(!tagfile.open(QIODevice::ReadOnly) && !vendorfile.open(QIODevice::ReadOnly))
            {
                QString msg(tagfile.errorString());
                msg.append(" and ");
                msg.append(vendorfile.errorString());
                QMessageBox::information(this, "Problem with rules file for both tags and vendors!", msg);
            }
            else
            {
                QTextStream tagIn(&tagfile);
                while(!tagIn.atEnd())
                {
                    changeInfo tagChangeInfo;
                    QString tagLine = tagIn.readLine();
                    if(tagLine.contains("change ", Qt::CaseSensitive))
                    {
                        QStringList taglineList = tagLine.split(" change ");
                        if(taglineList.size() == 2)
                        {
                            QString groupAndElement = taglineList.at(0);
                            tagChangeInfo.value = taglineList.at(1);
                            qDebug() << tagChangeInfo.value << endl;
                            if(groupAndElement.at(0) == '(' && groupAndElement.at(groupAndElement.length()-1) == ')')
                            {
                                groupAndElement = groupAndElement.right(groupAndElement.length()-1);
                                groupAndElement = groupAndElement.left(groupAndElement.length()-1);
                                QStringList tagSplitList = groupAndElement.split(", ");
                                if(tagSplitList.size() == 2)
                                {
                                    QString group = tagSplitList.at(0);
                                    QString element = tagSplitList.at(1);
                                    bool boolGroup;
                                    ushort shortGroup = group.toUShort(&boolGroup, 16);
                                    bool boolElement;
                                    ushort shortElement = element.toUShort(&boolElement, 16);
                                    if(boolGroup == true && boolElement == true)
                                    {
                                        tagChangeInfo.group = shortGroup;
                                        tagChangeInfo.element = shortElement;
                                        tagChangeInfo.changeStatus = 1;
                                        tagChangeInfo.errorMessage = "";
                                    }
                                    else
                                    {
                                        tagChangeInfo.errorMessage = "Error in group or element number provided!";
                                        tagChangeInfo.errorMessage.append(groupAndElement);
                                        tagChangeInfo.changeStatus = 2;
                                    }
                                }
                                else
                                {
                                    tagChangeInfo.errorMessage = "Something wrong with the comma(,), wrong separator!";
                                    tagChangeInfo.errorMessage.append(groupAndElement);
                                    tagChangeInfo.changeStatus = 2;
                                }
                            }
                            else
                            {
                                tagChangeInfo.errorMessage = "Something wrong with the brackets!";
                                tagChangeInfo.errorMessage.append(groupAndElement);
                                tagChangeInfo.changeStatus = 2;
                            }
                        }
                        else
                        {
                            tagChangeInfo.errorMessage = "Something wrong with the word following the delete keyword!";
                            tagChangeInfo.errorMessage.append(tagLine);
                            tagChangeInfo.changeStatus = 2;
                        }
                    }
                    else if(tagLine.contains(" delete", Qt::CaseSensitive))
                    {
                        QStringList taglineList = tagLine.split(" delete");
                        if(taglineList.size() == 2)
                        {
                            QString groupAndElement = taglineList.at(0);
                            tagChangeInfo.value = "";
                            if(groupAndElement.at(0) == '(' && groupAndElement.at(groupAndElement.length()-1) == ')')
                            {
                                groupAndElement = groupAndElement.right(groupAndElement.length()-1);
                                groupAndElement = groupAndElement.left(groupAndElement.length()-1);
                                QStringList tagSplitList = groupAndElement.split(", ");
                                if(tagSplitList.size() == 2)
                                {
                                    QString group = "0x";
                                    group.append(tagSplitList.at(0));
                                    QString element = "0x";
                                    element.append(tagSplitList.at(1));
                                    bool boolGroup;
                                    ushort shortGroup = group.toUShort(&boolGroup, 16);
                                    bool boolElement;
                                    ushort shortElement = element.toUShort(&boolElement, 16);
                                    if(boolGroup == true && boolElement == true)
                                    {
                                        tagChangeInfo.group = shortGroup;
                                        tagChangeInfo.element = shortElement;
                                        tagChangeInfo.changeStatus = 0;
                                        tagChangeInfo.errorMessage = "";
                                    }
                                    else
                                    {
                                        tagChangeInfo.errorMessage = "Error in group or element number provided!";
                                        tagChangeInfo.errorMessage.append(groupAndElement);
                                        tagChangeInfo.changeStatus = 2;
                                    }
                                }
                                else
                                {
                                    tagChangeInfo.errorMessage = "Something wrong with the comma(,), wrong separator!";
                                    tagChangeInfo.errorMessage.append(groupAndElement);
                                    tagChangeInfo.changeStatus = 2;
                                }
                            }
                            else
                            {
                                tagChangeInfo.errorMessage = "Something wrong with the brackets!";
                                tagChangeInfo.errorMessage.append(groupAndElement);
                                tagChangeInfo.changeStatus = 2;
                            }
                        }
                        else
                        {
                            tagChangeInfo.errorMessage = "Something wrong with the word following the delete keyword!";
                            tagChangeInfo.errorMessage.append(tagLine);
                            tagChangeInfo.changeStatus = 2;
                        }
                    }
                    else
                    {
                        tagChangeInfo.errorMessage = "Some wrong keyword used other than change or delete!";
                        tagChangeInfo.changeStatus = 2;
                    }
                    changeInfoList.append(tagChangeInfo);
                }
                tagfile.close();

                QTextStream vendorIn(&vendorfile);
                while(!vendorIn.atEnd())
                {
                    manufacturerImageStructure imageInfo;
                    QString vendorString = vendorIn.readLine();
                    QStringList vendorStringList = vendorString.split(" - ");
                    if(vendorStringList.size() == 2)
                    {
                        imageInfo.manufacturerName = vendorStringList.at(0);
                        QString vendorCoordinates = vendorStringList.at(1);
                        if(vendorCoordinates.at(0) == '('
                                && vendorCoordinates.at(vendorCoordinates.length()-1) == ')')
                        {
                            vendorCoordinates = vendorCoordinates.right(vendorCoordinates.length()-1);
                            vendorCoordinates = vendorCoordinates.left(vendorCoordinates.length()-1);
                            qDebug()<<vendorCoordinates<<endl;
                            QStringList coordList = vendorCoordinates.split(", ");
                            if(coordList.size()==4)
                            {
                                bool errorCheck = false;
                                for(int countCoord = 0; countCoord<coordList.size(); countCoord++)
                                {
                                    QStringList  colonList = coordList.at(countCoord).split(":");
                                    if(colonList.size()==2)
                                    {
                                        bool ok1, ok2;
                                        int firstNumber = colonList.at(0).toInt(&ok1, 10);
                                        int secondNumber = colonList.at(1).toInt(&ok2, 10);
                                        if(ok1 == true && ok2 == true)
                                        {
                                            if(countCoord == 0)
                                            {
                                                imageInfo.startX = firstNumber;
                                                imageInfo.verticalRatio = secondNumber;
                                            }
                                            else if(countCoord == 1)
                                            {
                                                imageInfo.startY = firstNumber;
                                                imageInfo.horizontalRatio = secondNumber;
                                            }
                                            else if(countCoord == 2)
                                            {
                                                imageInfo.endX = firstNumber;
                                            }
                                            else if(countCoord == 3)
                                            {
                                                imageInfo.endY = firstNumber;
                                            }
                                        }
                                        else
                                        {
                                            imageInfo.errorMessage = "Not a number, wrong input!";
                                            errorCheck = true;
                                        }
                                    }
                                    else
                                    {
                                        imageInfo.errorMessage = "Something wrong with the colon(:), wrong separator in ratto!";
                                        errorCheck = true;
                                    }
                                }
                                if(errorCheck == true)
                                {
                                    imageInfo.errorStatus = 1;
                                }
                                else
                                {
                                    imageInfo.errorStatus = 0;
                                }
                            }
                            else
                            {
                                imageInfo.errorMessage = "Something wrong with the comma(,), wrong separator!";
                                imageInfo.errorStatus = 1;
                            }
                        }
                        else
                        {
                            imageInfo.errorMessage = "Something wrong with the brackets!";
                            imageInfo.errorStatus = 1;
                        }
                    }
                    else
                    {
                        imageInfo.errorMessage = "No presence of G to indicate the coordinates!";
                        imageInfo.errorStatus = 1;
                    }
                    imgStructList.append(imageInfo);
                }
                vendorfile.close();
                QStringList sourceList;
                recurseAddDir(sourceDir, sourceList);
                QDir::toNativeSeparators(sourcepath);
                QDir::toNativeSeparators(destpath);
                bool fileSystemCreation = cpDir(sourcepath, destpath);
                //                for(int sourceListCounter = 0; sourceListCounter < sourceList.size(); sourceListCounter++)
                //                {
                //                    qDebug() << sourceList.at(sourceListCounter)<<endl;
                //                }
                //                qDebug() << "Number of files : "<<sourceList.size()<<endl;
                if(fileSystemCreation)
                {
                    QProgressDialog progress("Modifying dicom files...", "Cancel", 0, sourceList.size(), this);
                    progress.setWindowTitle("Progress bar");
                    progress.setWindowModality(Qt::WindowModal);
                    progress.setWindowFlags(Qt::CustomizeWindowHint  | Qt::WindowTitleHint | Qt::Window);
                    QString textFileName = destpath;
                    textFileName.append("\\modificationLog.txt");
                    QFile logFile(textFileName);
                    logFile.open(QIODevice::WriteOnly | QIODevice::Text);
                    QTextStream outLogFile(&logFile);
                    DJDecoderRegistration::registerCodecs(/*EDC_photometricInterpretation, EUC_default, EPC_default, OFFalse*/);// register JPEG codecs
                    for(int fileCounter = 0; fileCounter<sourceList.size(); fileCounter++)
                    {
                        progress.setValue(fileCounter);
                        if(progress.wasCanceled())
                        {
                            break;
                        }
                        QString logmessage;
                        DcmFileFormat fileformat;
                        OFCondition sourceLoadFile;
                        string sourceFilename = sourceList.at(fileCounter).toLocal8Bit().constData();
                        qDebug()<<sourceFilename.c_str()<<endl;
                        logmessage.append(sourceFilename.c_str());
                        sourceLoadFile = fileformat.loadFile(sourceFilename.c_str());
                        if(sourceLoadFile == EC_Normal)
                        {
                            DcmDataset *dataset = fileformat.getDataset();
                            for(int changeTagCounter = 0; changeTagCounter<changeInfoList.size(); changeTagCounter++)
                            {
                                if(changeInfoList.at(changeTagCounter).changeStatus != 2)
                                {
                                    DcmTag dTag(changeInfoList.at(changeTagCounter).group, changeInfoList.at(changeTagCounter).element);
                                    if(dataset->tagExists(dTag.getXTag()) == true)
                                    {
                                        if(changeInfoList.at(changeTagCounter).changeStatus == 0)
                                        {
                                            OFCondition deleteErr = dataset->findAndDeleteElement(dTag.getXTag(), OFTrue, OFTrue);
                                            if(deleteErr != EC_Normal)
                                            {
                                                QString errorMessage(QString(dTag.getTagName()));
                                                errorMessage.append(", ");
                                                errorMessage.append(deleteErr.text());
                                                QString tagString(dTag.toString().c_str());
                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), errorMessage.toLocal8Bit().constData(), 0, tagString.toLocal8Bit().constData());
                                            }
                                        }
                                        else if(changeInfoList.at(changeTagCounter).changeStatus == 1)
                                        {
                                            qDebug()<<dTag.getTagName()<<endl;
                                            OFCondition tempTagErr;
                                            QString inputValue;
                                            if(strcmpi("PatientBirthDate", dTag.getTagName()) == 0)
                                            {
                                                OFString DOB;
                                                tempTagErr = dataset->findAndGetOFString(dTag.getXTag(), DOB, 0, true);
                                                if(tempTagErr == EC_Normal)
                                                {
                                                    inputValue = DOB.c_str();
                                                    inputValue = inputValue.left(inputValue.length()-4);
                                                    inputValue.append("0101");
                                                }
                                                else
                                                {
                                                    inputValue = "19010101";
                                                }
                                            }
                                            else if(strcmpi("PatientName", dTag.getTagName()) == 0)
                                            {
                                                QString tagChangeType = changeInfoList.at(changeTagCounter).value;
                                                qDebug()<<"Name change option : "<<tagChangeType<<endl;
                                                QString patientSurname;
                                                QString patientGivenName;
                                                QStringList nameOptionSplitter = tagChangeType.split("^");
                                                if(nameOptionSplitter.size() == 2)
                                                {
                                                    if(QString::compare(nameOptionSplitter.at(0), "FolderName", Qt::CaseInsensitive) == 0)
                                                    {
                                                        QFileInfo file(sourceFilename.c_str());
                                                        patientGivenName = file.dir().dirName();
                                                    }
                                                    else if(QString::compare(nameOptionSplitter.at(0), "FileName", Qt::CaseInsensitive) == 0)
                                                    {
                                                        QFileInfo file(sourceFilename.c_str());
                                                        patientGivenName = file.baseName();
                                                    }
                                                    else
                                                    {
                                                        patientGivenName = nameOptionSplitter.at(0);
                                                    }

                                                    if(QString::compare(nameOptionSplitter.at(1), "FolderName", Qt::CaseInsensitive) == 0)
                                                    {
                                                        QFileInfo file(sourceFilename.c_str());
                                                        patientSurname = file.dir().dirName();
                                                    }
                                                    else if(QString::compare(nameOptionSplitter.at(1), "FileName", Qt::CaseInsensitive) == 0)
                                                    {
                                                        QFileInfo file(sourceFilename.c_str());
                                                        patientSurname = file.baseName();
                                                    }
                                                    else
                                                    {
                                                        patientSurname = nameOptionSplitter.at(1);
                                                    }
                                                }
                                                else
                                                {
                                                    patientGivenName = tagChangeType.left((int)(tagChangeType.length()/2));
                                                    patientSurname = tagChangeType.right((int)(tagChangeType.length()/2));
                                                }
                                                inputValue.append(patientGivenName);
                                                inputValue = inputValue.append("^");
                                                inputValue.append(patientSurname);
                                                qDebug()<<inputValue<<endl;
                                                OFString patientName;
                                                logmessage.append(", Old Patient name: ");
                                                tempTagErr = dataset->findAndGetOFString(dTag.getXTag(), patientName, 0, true);
                                                if(tempTagErr == EC_Normal)
                                                {
                                                    logmessage.append(patientName.c_str());
                                                }
                                                else
                                                {
                                                    logmessage.append("Name could not be extracted");
                                                }
                                                logmessage.append(", New Patient name: ");
                                                logmessage.append(inputValue);
                                            }
                                            else
                                            {
                                                inputValue = changeInfoList.at(changeTagCounter).value;
                                            }
                                            OFCondition changeErr = dataset->putAndInsertString(dTag.getXTag(), inputValue.toLocal8Bit().constData(), OFTrue);
                                            if(changeErr != EC_Normal)
                                            {
                                                QString errorMessage(QString(dTag.getTagName()));
                                                errorMessage.append(", ");
                                                errorMessage.append(changeErr.text());
                                                QString tagString(dTag.toString().c_str());
                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), errorMessage.toLocal8Bit().constData(), 0, tagString.toLocal8Bit().constData());
                                            }
                                        }
                                    }
                                    else
                                    {
                                        QString errorMessage(QString(dTag.getTagName()));
                                        errorMessage.append(", ");
                                        errorMessage.append("tag doesn't exist!");
                                        QString tagString(dTag.toString().c_str());
                                        errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), errorMessage.toLocal8Bit().constData(), 0, tagString.toLocal8Bit().constData());
                                    }
                                }
                                else
                                {
                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), changeInfoList.at(changeTagCounter).errorMessage.toLocal8Bit().constData(), 0, "");
                                }
                            }
                            bool pixelModificationErrorCheck = false;
                            E_TransferSyntax xfer= dataset->getOriginalXfer();
                            OFString manufacturer;
                            OFCondition getManufacturerErr = dataset->findAndGetOFString(DCM_Manufacturer, manufacturer, 0, OFTrue);
                            if(getManufacturerErr == EC_Normal)
                            {
                                int imgStructRef = -5;
                                for(int imgStructCounter = 0; imgStructCounter<imgStructList.size(); imgStructCounter++)
                                {
                                    if(strcmpi(manufacturer.c_str(), imgStructList.at(imgStructCounter).manufacturerName.toLocal8Bit().constData()) == 0)
                                    {
                                        imgStructRef = imgStructCounter;
                                        break;
                                    }
                                }
                                if(imgStructRef != -5)
                                {
                                    OFBool pWriteYBR422 = OFFalse;
                                    const char* photoMetInter;
                                    OFCondition photometInt = dataset->findAndGetString(DCM_PhotometricInterpretation, photoMetInter, true);
                                    if(photometInt == EC_Normal)
                                    {
                                        if(strcmpi(photoMetInter, "YBR_FULL_422")==0)
                                        {
                                            pWriteYBR422 = OFTrue;
                                        }

                                        rows = 0;
                                        cols = 0;
                                        OFCondition rowCheck=dataset->findAndGetUint16(DCM_Rows, rows);
                                        if(EC_Normal == rowCheck)
                                        {
                                            OFCondition colCheck=dataset->findAndGetUint16(DCM_Columns, cols);
                                            if(EC_Normal == colCheck)
                                            {
                                                int verticalStepSize = cols/imgStructList.at(imgStructRef).verticalRatio;
                                                int horizontalStepSize = rows/imgStructList.at(imgStructRef).horizontalRatio;
                                                OFCondition chRep = dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
                                                if(EC_Normal == chRep)
                                                {
                                                    if(dataset->canWriteXfer(EXS_LittleEndianExplicit)==true)
                                                    {
                                                        DcmElement* element=NULL;
                                                        OFCondition DPixData = dataset->findAndGetElement(DCM_PixelData,element);
                                                        if(EC_Normal==DPixData)
                                                        {
                                                            samplePerPixel = 0;
                                                            bool planarConfigurationExists = false;
                                                            Uint16 planarConfiguration = 0;
                                                            Uint32 startFragment=0;
                                                            sizeF=0;
                                                            OFCondition UcFrameS=element->getUncompressedFrameSize(dataset,sizeF);
                                                            if(EC_Normal==UcFrameS)
                                                            {
                                                                OFString decompressedColorModel=NULL;
                                                                OFCondition DecColorM=element->getDecompressedColorModel(dataset,decompressedColorModel);
                                                                if(EC_Normal==DecColorM)
                                                                {
                                                                    long int  numberOfFrames=0;
                                                                    OFCondition samplePP=dataset->findAndGetUint16(DCM_SamplesPerPixel, samplePerPixel);
                                                                    if(EC_Normal==samplePP)
                                                                    {
                                                                        bool goOn = true;
                                                                        QString tempErrMsg;
                                                                        if(dataset->tagExistsWithValue(DCM_NumberOfFrames))
                                                                        {
                                                                            OFCondition numOfFrameErr=dataset->findAndGetLongInt(DCM_NumberOfFrames,numberOfFrames);
                                                                            if(numOfFrameErr==EC_Normal)
                                                                            {
                                                                                if(numberOfFrames<1)
                                                                                {
                                                                                    numberOfFrames=1;
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                tempErrMsg.append(QString(numOfFrameErr.text()));
                                                                                goOn = false;
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            numberOfFrames = 1;
                                                                        }
                                                                        if(goOn == true)
                                                                        {
                                                                            try
                                                                            {
                                                                                bool alright = false;
                                                                                Uint8 * fullBuffer = new Uint8[(int(sizeF*numberOfFrames))];
                                                                                Uint16 bitsAllocated;
                                                                                OFCondition bitsAlloErr = dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
                                                                                if(bitsAlloErr == EC_Normal)
                                                                                {
                                                                                    if(samplePerPixel == 3)
                                                                                    {
                                                                                        if((int)bitsAllocated == 8)
                                                                                        {
                                                                                            if(dataset->tagExistsWithValue(DCM_PlanarConfiguration))
                                                                                            {
                                                                                                planarConfigurationExists=true;
                                                                                                OFCondition PConf=dataset->findAndGetUint16(DCM_PlanarConfiguration,planarConfiguration);
                                                                                                if(EC_Normal==PConf)
                                                                                                {
                                                                                                    if(int(planarConfiguration) == 0)
                                                                                                    {
                                                                                                        int xmin = imgStructList.at(imgStructRef).startX*verticalStepSize;
                                                                                                        int xmax = imgStructList.at(imgStructRef).endX*verticalStepSize;
                                                                                                        int ymin = imgStructList.at(imgStructRef).startY*horizontalStepSize;
                                                                                                        int ymax = imgStructList.at(imgStructRef).endY*horizontalStepSize;
                                                                                                        for(int frameCounter=0; frameCounter<numberOfFrames; frameCounter++)
                                                                                                        {
                                                                                                            Uint8 * tBuffer = new Uint8[int(sizeF)];//Buffer for each frame
                                                                                                            DcmFileCache * cache=NULL;
                                                                                                            OFCondition UFrame=element->getUncompressedFrame(dataset, frameCounter, startFragment, tBuffer, sizeF, decompressedColorModel, cache);
                                                                                                            if(EC_Normal==UFrame)
                                                                                                            {
                                                                                                                if(tBuffer != NULL)
                                                                                                                {
                                                                                                                    if(xmin<(int)cols && ymin<(int)rows)
                                                                                                                    {
                                                                                                                        pl0pixelReplace(tBuffer, xmax, ymax, xmin, ymin);
                                                                                                                    }
                                                                                                                    memcpy(fullBuffer+(frameCounter*sizeF), tBuffer,sizeF);
                                                                                                                }
                                                                                                                alright = true;
                                                                                                                delete [] tBuffer;
                                                                                                            }
                                                                                                            else
                                                                                                            {
                                                                                                                alright = false;
                                                                                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), UFrame.text(), 0, "In getting uncompressed frame");
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                    else if(int(planarConfiguration) == 1)
                                                                                                    {
                                                                                                        const Uint8 * tempBuf = 0;
                                                                                                        unsigned long * count = NULL;
                                                                                                        OFCondition fullBuf = dataset->findAndGetUint8Array(DCM_PixelData, tempBuf, count, OFFalse);
                                                                                                        if(fullBuf == EC_Normal)
                                                                                                        {
                                                                                                            int xmin = imgStructList.at(imgStructRef).startX*verticalStepSize;
                                                                                                            int xmax = imgStructList.at(imgStructRef).endX*verticalStepSize;
                                                                                                            int ymin = imgStructList.at(imgStructRef).startY*horizontalStepSize;
                                                                                                            int ymax = imgStructList.at(imgStructRef).endY*horizontalStepSize;
                                                                                                            memcpy(fullBuffer, tempBuf,sizeF*numberOfFrames);
                                                                                                            for( int frameCounter=0; frameCounter<numberOfFrames; frameCounter++)
                                                                                                            {
                                                                                                                if(xmin<(int)cols && ymin<(int)rows)
                                                                                                                {
                                                                                                                    pl1pixelReplace(fullBuffer, xmax, ymax, xmin, ymin, frameCounter);
                                                                                                                }
                                                                                                            }
                                                                                                            alright = true;
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                            delete [] fullBuffer;
                                                                                                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), fullBuf.text(), 0, "(7FE0, 0010)");
                                                                                                        }
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        delete [] fullBuffer;
                                                                                                        errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), "Unknown planar configuration!", 0, "In planar configuration");
                                                                                                    }
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    delete [] fullBuffer;
                                                                                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), PConf.text(), 0, "(0028, 0006)");
                                                                                                }
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                delete [] fullBuffer;
                                                                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), "Planar Configuration tag doesn't exist!", 0, "(0x0028, 0x0006)");
                                                                                            }
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            delete [] fullBuffer;
                                                                                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), "RGB image with only 8 bit allocation is supported.", 0, "(0028, 0004)");
                                                                                        }
                                                                                    }
                                                                                    else if(samplePerPixel == 1)
                                                                                    {
                                                                                        Uint16 bitStored;
                                                                                        OFCondition bitsStorErr = dataset->findAndGetUint16(DCM_BitsStored, bitStored);
                                                                                        if(bitsStorErr == EC_Normal)
                                                                                        {
                                                                                            double factor = (int)sizeF/((int)rows * (int) cols * (int) samplePerPixel);
                                                                                            double result = (cols*factor);
                                                                                            int xmin = factor*imgStructList.at(imgStructRef).startX*verticalStepSize;
                                                                                            int xmax = factor*imgStructList.at(imgStructRef).endX*verticalStepSize;
                                                                                            int ymin = imgStructList.at(imgStructRef).startY*horizontalStepSize;
                                                                                            int ymax = imgStructList.at(imgStructRef).endY*horizontalStepSize;
                                                                                            for(int frameCounter=0; frameCounter<numberOfFrames; frameCounter++)
                                                                                            {
                                                                                                Uint8 * tBuffer = new Uint8[int(sizeF)];//Buffer for each frame
                                                                                                DcmFileCache * cache=NULL;
                                                                                                OFCondition UFrame=element->getUncompressedFrame(dataset, frameCounter, startFragment, tBuffer, sizeF, decompressedColorModel, cache);
                                                                                                if(EC_Normal==UFrame)
                                                                                                {
                                                                                                    if(tBuffer != NULL)
                                                                                                    {
                                                                                                        if(xmin<(int)result && factor*ymin<rows)
                                                                                                        {
                                                                                                            nonPlanarPixelReplace(tBuffer, xmax, ymax, xmin, ymin, factor);
                                                                                                        }
                                                                                                    }
                                                                                                    delete [] tBuffer;
                                                                                                    alright = true;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    alright = false;
                                                                                                    delete [] tBuffer;
                                                                                                    delete [] fullBuffer;
                                                                                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), UFrame.text(), 0, "In getting uncompressed data!");
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            delete [] fullBuffer;
                                                                                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), bitsStorErr.text(), 0, "(0028, 0101)");
                                                                                        }

                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    delete [] fullBuffer;
                                                                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), bitsAlloErr.text(), 0, "(0028, 0004)");
                                                                                }
                                                                                if(alright == true)
                                                                                {
                                                                                    OFCondition fullcopy=dataset->putAndInsertUint8Array(DCM_PixelData,fullBuffer,sizeF*numberOfFrames,true);
                                                                                    if(fullcopy == EC_Normal)
                                                                                    {
                                                                                        pixelModificationErrorCheck = true;
                                                                                        delete [] fullBuffer;
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        delete [] fullBuffer;
                                                                                        errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), fullcopy.text(), 0, "(7FE0, 0010)");
                                                                                    }
                                                                                }
                                                                            }
                                                                            catch(exception &e)
                                                                            {
                                                                                const char* msg = e.what();
                                                                                QString message(msg);
                                                                                message.append(" during memory allocation!");
                                                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), message.toLocal8Bit().constData(), 0, "No tag");
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), tempErrMsg.toLocal8Bit().constData(), 0, "(0028, 0008)");
                                                                        }

                                                                    }
                                                                    else
                                                                    {
                                                                        errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), samplePP.text(), 0, "(0x0028, 0x0002)");
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), DecColorM.text(), 0, "No tag");
                                                                }
                                                            }
                                                            else
                                                            {
                                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), UcFrameS.text(), 0, "No tag");

                                                            }
                                                        }
                                                        else
                                                        {
                                                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), DPixData.text(), 0, "(7FE0, 0010)");
                                                        }
                                                        DJEncoderRegistration::registerCodecs(ECC_lossyYCbCr, EUC_default, OFFalse, 0, 0, 0, OFTrue, ESS_444, pWriteYBR422, OFFalse, 0, 0, 0.0, 0.0, 0, 0, 0, 0, OFTrue, OFFalse, OFFalse, OFFalse, OFTrue);
                                                        try
                                                        {
                                                            OFCondition compressing;
                                                            if(xfer == 12 || xfer == 11 || xfer == 24 || xfer == 19 || xfer == 20 || xfer == 21 || xfer == 22 || xfer == 23 || xfer == 25 || xfer == 26 || xfer == 30)
                                                            {
                                                                DJ_RPLossless params;
                                                                compressing = dataset->chooseRepresentation(xfer, &params);
                                                                if(compressing != EC_Normal)
                                                                {
                                                                    pixelModificationErrorCheck = false;
                                                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), compressing.text(), 0, "In re-compressing");
                                                                }
                                                            }
                                                            // Still compression methods 28, 29, 31, 32, 33 not covered. Potential bug exists.
                                                            else if(xfer == 4 || xfer == 5 || xfer == 6 || xfer == 7 || xfer == 8 || xfer == 9 || xfer == 10 || xfer == 13 || xfer == 14 || xfer == 15 || xfer == 16 || xfer == 17 || xfer == 18 || xfer == 27)
                                                            {
                                                                DJ_RPLossy params;
                                                                compressing = dataset->chooseRepresentation(xfer, &params);
                                                                if(compressing != EC_Normal)
                                                                {
                                                                    pixelModificationErrorCheck = false;
                                                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), compressing.text(), 0, "In re-compressing");
                                                                }
                                                            }
                                                            else
                                                            {

                                                                pixelModificationErrorCheck = false;
                                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), "Transfer sytax not supported!", 0, "In re-compressing");
                                                            }
                                                        }
                                                        catch(exception &e1)
                                                        {
                                                            pixelModificationErrorCheck = false;
                                                            const char* msg = e1.what();
                                                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), msg, 0, "In re-compressing the pixel data!");
                                                        }
                                                        DJEncoderRegistration::cleanup();
                                                    }
                                                    else
                                                    {
                                                        errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), "Transfer syntax cannot be modified as decompression is not possible.", 0, "In decompressing!");
                                                    }
                                                }
                                                else
                                                {
                                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), chRep.text(), 0, "in choosing representation!");
                                                }
                                            }
                                            else
                                            {
                                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), rowCheck.text(), 0, "(0028, 0011)");
                                            }
                                        }
                                        else
                                        {
                                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), rowCheck.text(), 0, "(0028, 0010)");
                                        }
                                    }
                                    else
                                    {
                                        errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), photometInt.text(), 0, "(0028, 0004)");
                                    }
                                }
                                else
                                {
                                    errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), "This manufacturer name is not in the list!", 0, "In reading the manufacturer name!");
                                }
                            }
                            else
                            {
                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), getManufacturerErr.text(), 0, "DCM_ManufacturerName!");
                            }
                            QString convertedFileName = sourceList.at(fileCounter);
                            convertedFileName = convertedFileName.right(convertedFileName.length()-sourcepath.length());
                            convertedFileName = destpath + convertedFileName;
                            qDebug()<<convertedFileName<<endl;
                            OFCondition newCond=fileformat.saveFile(convertedFileName.toLocal8Bit().constData(), xfer);
                            if(newCond==EC_Normal)
                            {
                                fileformat.clear();
                                dataset->clear();
                            }
                            else
                            {
                                fileformat.clear();
                                dataset->clear();
                                errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), newCond.text(), 0, "In saving!");
                            }
                            if(pixelModificationErrorCheck == false)
                            {
                                logmessage.append(", file deleted because of error!");
                                QFile tobeDeleted(convertedFileName);
                                tobeDeleted.remove();
                            }
                        }
                        else
                        {
                            logmessage.append(", File failed to be loaded!");
                            fileformat.clear();
                            errorFileLog(sourceList.at(fileCounter).toLocal8Bit().constData(), sourceLoadFile.text(), 0, "In loading the file!");
                        }
                        logmessage.append("\n");
                        outLogFile<<logmessage;
                    }
                    progress.setValue(sourceList.size());
                    logFile.close();
                    DJDecoderRegistration::cleanup();
                }
                else
                {
                    QMessageBox::information(this, "File modfication status!", "Directory cannot be refreshed!");
                    sourceDir.cdUp();
                    sourceDir.mkpath(destpath);
                }
            }
        }
    }
}

bool MainWindow::rmDir(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists())
        return true;
    foreach(const QFileInfo &info, dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
        if (info.isDir()) {
            if (!rmDir(info.filePath()))
                return false;
        } else {
            if (!dir.remove(info.fileName()))
                return false;
        }
    }
    QDir parentDir(QFileInfo(dirPath).path());
    bool internalRmDir = parentDir.rmdir(QFileInfo(dirPath).fileName());
    return internalRmDir;
}

bool MainWindow::cpDir(const QString &srcPath, const QString &dstPath)
{
    bool rmDirCheck = rmDir(dstPath);
    if(rmDirCheck == true)
    {
        QDir parentDstDir(QFileInfo(dstPath).path());
        if (!parentDstDir.mkdir(QFileInfo(dstPath).fileName()))
            return false;

        QDir srcDir(srcPath);
        foreach(const QFileInfo &info, srcDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot)) {
            QString srcItemPath = srcPath + "/" + info.fileName();
            QString dstItemPath = dstPath + "/" + info.fileName();
            if (info.isDir()) {
                if (!cpDir(srcItemPath, dstItemPath)) {
                    return false;
                }
            } else if (info.isFile()) {
                if (!QFile::copy(srcItemPath, dstItemPath)) {
                    return false;
                }
            } else {
                qDebug() << "Unhandled item" << info.filePath() << "in cpDir";
            }
        }
        return true;
    }
    else
    {
        return rmDirCheck;
    }
}


void MainWindow::recurseAddDir(QDir d, QStringList & list) {

    QStringList qsl = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

    foreach (QString file, qsl) {

        QFileInfo finfo(QString("%1/%2").arg(d.path()).arg(file));

        if (finfo.isSymLink())
        {
            qDebug()<<"System like with"<<finfo.fileName()<<"\n";
            return;
        }
        if (finfo.isDir()) {

            QString dirname = finfo.fileName();
            QDir sd(finfo.filePath());

            recurseAddDir(sd, list);

        } else
            list << QDir::toNativeSeparators(finfo.filePath());
    }
}

void MainWindow::tagChanging()
{
    if(correctList.size()>0 || defectedPixelData.size()>0)
    {
        if(selectedImages.size()>0 || (/*correctList.size()==0 && */selectedImages.size()==0 && defectedPixelData.size()>0))
        {
            bool defectTask = false;
            DcmFileFormat fileformat;
            OFCondition sourceLoadFile;
            QString txtFileName;
            if(correctList.size()>0 && selectedImages.size()>0)
            {
                sourceLoadFile = fileformat.loadFile(correctList.at(selectedImages.at(0)).toLocal8Bit().constData());
                txtFileName = correctList.at(selectedImages.at(0));
                txtFileName.append(".txt");
            }
            else if(selectedImages.size()==0 && defectedPixelData.size()>0)
            {
                sourceLoadFile = fileformat.loadFile(defectedPixelData.at(0).toLocal8Bit().constData());
                txtFileName = defectedPixelData.at(0);
                txtFileName.append(".txt");
                if(correctList.size() > 0)
                {
                    QMessageBox::information(this, "Warning!", "You did not select any file from the visualized list. So, only the headers of non-visualized list will be de-identified!");
                }
                defectTask = true;
            }
            if(sourceLoadFile == EC_Normal)
            {
                tagDialog = new tableDialog(this);
                DcmDataset *dataset = fileformat.getDataset();
                ofstream outFile (txtFileName.toLocal8Bit().constData(), ofstream::out);
                size_t printFlags = DCMTypes::PF_shortenLongTagValues;
                dataset->print(outFile, printFlags, 0, NULL, NULL);
                outFile.close();
                ifstream inFile(txtFileName.toLocal8Bit().constData(), ifstream::in);
                string line = "";
                if(inFile.is_open())
                {
                    tagDialog->model = new QStandardItemModel(1,4,tagDialog);
                    tagDialog->model->setHeaderData(0, Qt::Horizontal, "Group", Qt::DisplayRole);
                    tagDialog->model->setHeaderData(1, Qt::Horizontal, "Element", Qt::DisplayRole);
                    tagDialog->model->setHeaderData(2, Qt::Horizontal, "Tag Description", Qt::DisplayRole);
                    tagDialog->model->setHeaderData(3, Qt::Horizontal, "Value", Qt::DisplayRole);
                    int tagTableCounter = 0;
                    while(!inFile.eof())
                    {
                        getline(inFile, line);
                        if(strchr(line.c_str(),'(')!=NULL && strchr(line.c_str(),')')!=NULL && strchr(line.c_str(),'#')!=NULL)
                        {
                            const char* first = strchr(line.c_str(), '(');
                            const char* second = strchr(line.c_str(), ')');
                            int firstPos = first-line.c_str();
                            int secondPos = second-line.c_str();
                            if((secondPos-firstPos)==10)
                            {
                                char tag[12];
                                strncpy(tag, line.c_str()+firstPos, 11);
                                tag[11]='\0';
                                if(strcmpi(tag, "(7fe0,0010)") == 0)
                                {
                                    inFile.close();
                                    break;
                                }
                                if(strcmpi(tag, "(fffe,e000)")!=0 && strcmpi(tag, "(fffe,e00d)")!=0 && strcmpi(tag, "(fffe,e0dd)")!=0 && strcmpi(tag, "(0020,000D)")!=0 && strcmpi(tag, "(0020,000E)")!=0)
                                {
                                    char group[5];
                                    char element[5];
                                    strncpy(group, tag+1, 4);
                                    group[4] = '\0';
                                    QString qGroup = "0x";
                                    qGroup.append(group);
                                    strncpy(element, tag+6, 4);
                                    element[4] = '\0';
                                    QString qElement = "0x";
                                    qElement.append(element);
                                    bool boolGroup;
                                    ushort shortGroup = qGroup.toUShort(&boolGroup, 16);
                                    bool boolElement;
                                    ushort shortElement = qElement.toUShort(&boolElement, 16);
                                    if(boolGroup == true && boolElement == true)
                                    {
                                        DcmTag dTag(shortGroup, shortElement);
                                        const char* tagName = dTag.getTagName();
                                        OFString tagValue;
                                        OFCondition nameErr = dataset->findAndGetOFString(dTag.getXTag(), tagValue, 0, true);
                                        if(nameErr == EC_Normal)
                                        {
                                            QStandardItem * groupItm = new QStandardItem;
                                            groupItm->setText(QString(group));
                                            groupItm->setFlags(groupItm->flags() ^ Qt::ItemIsEditable);
                                            tagDialog->model->setItem(tagTableCounter, 0, groupItm);
                                            QStandardItem * elementItm = new QStandardItem;
                                            elementItm->setText(QString(element));
                                            elementItm->setFlags(elementItm->flags() ^ Qt::ItemIsEditable);
                                            tagDialog->model->setItem(tagTableCounter, 1, elementItm);
                                            QStandardItem * tagDesItm = new QStandardItem;
                                            tagDesItm->setText(QString(tagName));
                                            tagDesItm->setFlags(tagDesItm->flags() ^ Qt::ItemIsEditable);
                                            tagDialog->model->setItem(tagTableCounter, 2, tagDesItm);
                                            QStandardItem * tagValItm = new QStandardItem;
                                            tagValItm->setText(QString(tagValue.c_str()));
                                            tagDialog->model->setItem(tagTableCounter, 3, tagValItm);
                                            tagModelStarted = true;
                                            tagTableCounter++;
                                        }
                                        qGroup.clear();
                                        qElement.clear();
                                    }
                                }
                            }
                        }
                    }
                    QGridLayout form(tagDialog);
                    form.addWidget(tagDialog->tagData, 0, 0, 5, 5, Qt::AlignLeft);
                    form.addWidget(tagDialog->OkButton, 8, 0, 1, 1, Qt::AlignLeft);
                    form.addWidget(tagDialog->CancelButton, 8, 1, 1, 1, Qt::AlignLeft);
                    bool validEmpty = false;
                    if(defectedPixelData.size()>0 && selectedImages.size()>0)
                    {
                        form.addWidget(tagDialog->takeDefectedImage, 6, 1, 1, 1, Qt::AlignLeft);
                    }
                    if(selectedImages.size()>0)
                    {
                        form.addWidget(tagDialog->modifyMode, 6, 0, 1, 1, Qt::AlignLeft);
                    }
                    else if(selectedImages.size()==0 && defectedPixelData.size()>0)
                    {
                        validEmpty = true;
                    }
                    form.addWidget(tagDialog->uidChangeIndicator, 7, 0, 1, 1, Qt::AlignLeft);
                    tagDialog->tagData->setModel(tagDialog->model);
                    tagDialog->tagData->verticalHeader()->setVisible(false);
                    tagDialog->tagData->resizeColumnsToContents();
                    tagDialog->tagData->show();
                    connect(tagDialog->OkButton, SIGNAL(clicked()), tagDialog, SLOT(accept()));
                    connect(tagDialog->CancelButton, SIGNAL(clicked()), tagDialog, SLOT(reject()));
                    connect(tagDialog->tagData, SIGNAL(pressed(const QModelIndex&)), this, SLOT(mediator()));
                    int execStatus = tagDialog->exec();
                    if(execStatus == QDialog::Accepted)
                    {
                        for(int listCounter = 0; listCounter<changeList.size(); listCounter++)
                        {
                            tagNames.append(tagDetail(tagDialog->model->item(changeList.at(listCounter), 0)->text(), tagDialog->model->item(changeList.at(listCounter), 1)->text(), tagDialog->model->item(changeList.at(listCounter), 3)->text()));
                        }
                        tagModelStarted = false;
                        if(destinationSelected == false)
                        {
                            QString pathBuild;
                            QDir dir;
                            QStringList splitStr;
                            if(correctList.size()>0 && selectedImages.size()>0)
                            {
                                splitStr = correctList.at(selectedImages.at(0)).split("/");
                            }
                            else if(selectedImages.size()==0 && defectedPixelData.size()>0)
                            {
                                splitStr = defectedPixelData.at(0).split("/");
                            }
                            for(int wordCounter = 0; wordCounter<splitStr.size()-1; wordCounter++)
                            {
                                pathBuild.append(splitStr.at(wordCounter));
                                pathBuild.append("/");
                            }
                            dir.setPath(pathBuild);
                            pathBuild.append("Modified Files");
                            QString destPath = pathBuild;
                            if(dir.exists(destPath)==false)
                            {
                                bool fileConstruct = dir.mkdir(destPath);
                                if(fileConstruct == true)
                                {
                                    destDirectory.setPath(destPath);
                                    destinationSelected = true;
                                }
                                else
                                {
                                    QMessageBox::information(this, "Message!", "No permission to write in the desired directory!");
                                }
                            }
                            else
                            {
                                destDirectory.setPath(destPath);
                                destinationSelected = true;
                            }
                        }
                        if(destinationSelected == true)
                        {
                            UIDChanges = false;
                            if(tagDialog->uidChangeIndicator->isChecked())
                            {
                                IDListFillup();
                                UIDChanges = true;
                            }
                            if(tagDialog->takeDefectedImage->isChecked() || validEmpty == true)
                            {
                                if(defectedPixelData.size()>0)
                                {
                                    bool alright = false;
                                    string hash = "/defected files";
                                    string oldDir = destDirectory.absolutePath().toLocal8Bit().constData();
                                    string newDir = oldDir + hash;
                                    if(destDirectory.exists()==true)
                                    {
                                        if(destDirectory.exists(newDir.c_str()) == false)
                                        {
                                            if(destDirectory.mkdir(newDir.c_str()))
                                            {
                                                alright = true;
                                            }
                                        }
                                        else
                                        {
                                            alright = true;
                                        }
                                    }
                                    if(alright == true)
                                    {
                                        int changeCounter = 0;
                                        QProgressDialog progress("Modifying dicom headers of files that have defective pixel data...", "Cancel", 0, defectedPixelData.size(), this);
                                        progress.setWindowTitle("Progress bar");
                                        progress.setWindowModality(Qt::WindowModal);
                                        progress.setWindowFlags(Qt::CustomizeWindowHint  | Qt::WindowTitleHint | Qt::Window);
                                        while(changeCounter<defectedPixelData.size())
                                        {
                                            progress.setValue(changeCounter);
                                            if(progress.wasCanceled())
                                            {
                                                break;
                                            }
                                            MdfDatasetManager modifyDataset;
                                            OFCondition loadCond = modifyDataset.loadFile(defectedPixelData.at(changeCounter).toLocal8Bit().constData(),ERM_autoDetect,EXS_Unknown);
                                            if(loadCond==EC_Normal)
                                            {
                                                int count=0;
                                                for(int modifyCounter = 0; modifyCounter<tagNames.size(); modifyCounter++)
                                                {
                                                    QString fullTag = "(";
                                                    fullTag.append(tagNames.at(modifyCounter).groupNumber);
                                                    fullTag.append(",");
                                                    fullTag.append(tagNames.at(modifyCounter).elementNumber);
                                                    fullTag.append(")");
                                                    OFCondition tempError = modifyDataset.modifyAllTags(fullTag.toLocal8Bit().constData(), tagNames.at(modifyCounter).value.toLocal8Bit().constData(), OFTrue, count, OFFalse);
                                                    if(tempError != EC_Normal)
                                                    {
                                                        errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), tempError.text(), 0, fullTag.toLocal8Bit().constData());
                                                    }
                                                }
                                                if(UIDChanges == true)
                                                {
                                                    if(seriesInstanceListDF.size()==defectedPixelData.size())
                                                    {
                                                        if(QString::compare(seriesInstanceListDF.at(changeCounter).fileNames, defectedPixelData.at(changeCounter), Qt::CaseInsensitive) == 0 &&
                                                                QString::compare(seriesInstanceListDF.at(changeCounter).UID, QString("Error"), Qt::CaseInsensitive)!=0)
                                                        {
                                                            OFCondition seriesUIDErr = modifyDataset.modifyAllTags("(0020,000E)", seriesInstanceListDF.at(changeCounter).UID.toLocal8Bit().constData(), OFTrue, count, OFFalse);
                                                            if(seriesUIDErr != EC_Normal)
                                                            {
                                                                errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), seriesUIDErr.text(), 0, "(0020,000E)");
                                                            }
                                                        }
                                                        else
                                                        {
                                                            errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), "Error in generating new Series Instance UID", 0, "(0020,000E)");
                                                        }
                                                    }
                                                    else
                                                    {
                                                        errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), "Error in generating new Series Instance UID", 0, "(0020,000E)");
                                                    }
                                                    if(studyInstanceListDF.size()==defectedPixelData.size())
                                                    {
                                                        if(QString::compare(studyInstanceListDF.at(changeCounter).fileNames, defectedPixelData.at(changeCounter), Qt::CaseInsensitive) == 0 &&
                                                                QString::compare(studyInstanceListDF.at(changeCounter).UID, QString("Error"), Qt::CaseInsensitive)!=0)
                                                        {
                                                            OFCondition studyUIDErr = modifyDataset.modifyAllTags("(0020,000D)", studyInstanceListDF.at(changeCounter).UID.toLocal8Bit().constData(), OFTrue, count, OFFalse);
                                                            if(studyUIDErr != EC_Normal)
                                                            {
                                                                errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), studyUIDErr.text(), 0, "(0020,000D)");
                                                            }
                                                        }
                                                        else
                                                        {
                                                            errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), "Error in generating new Study Instance UID", 0, "(0020,000D)");
                                                        }

                                                    }
                                                    else
                                                    {
                                                        errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), "Error in generating new Study Instance UID", 0, "(0020,000D)");
                                                    }
                                                }
                                                string destinationFilename;
                                                string hash="/";
                                                destinationFilename=newDir.c_str()+hash+defectedPixelData.at(changeCounter).split("/").last().toLocal8Bit().constData();
                                                OFCondition saveError = modifyDataset.saveFile(destinationFilename.c_str()/*, xfer,EET_UndefinedLength,EGL_recalcGL,EPD_noChange,0,0,OFFalse*/);
                                                if(saveError != EC_Normal)
                                                {
                                                    QString errorMessage(QString("Saving file, "));
                                                    errorMessage.append(saveError.text());
                                                    errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), errorMessage.toLocal8Bit().constData(), 0, "No tag");
                                                }
                                                else if(saveError == EC_Normal)
                                                {
                                                    finishedList<<destinationFilename.c_str();
                                                    ui->finishedList->clear();
                                                    ui->finishedList->addItems(finishedList);
                                                    ui->finishedList->show();
                                                }
                                            }
                                            else
                                            {
                                                QString errorMessage(QString("Loading file, "));
                                                errorMessage.append(loadCond.text());
                                                errorFileLog(defectedPixelData.at(changeCounter).toLocal8Bit().constData(), errorMessage.toLocal8Bit().constData(), 0, "No tag");
                                            }
                                            changeCounter++;
                                        }
                                        progress.setValue(defectedPixelData.size());
                                    }
                                    else
                                    {
                                        QMessageBox::information(this, "Message", "No permission to create the sub folder for defective files!");
                                    }
                                }
                            }
                            if(validEmpty == false)
                            {
                                if(tagDialog->modifyMode->isChecked())
                                {
                                    int listSize = selectedImages.size();
                                    QProgressDialog progress("Modifying dicom headers...", "Cancel", 0, listSize, this);
                                    progress.setWindowTitle("Progress bar");
                                    progress.setWindowModality(Qt::WindowModal);
                                    progress.setWindowFlags(Qt::CustomizeWindowHint  | Qt::WindowTitleHint | Qt::Window);
                                    int measure = 0;
                                    while(selectedImages.size()>0)
                                    {
                                        progress.setValue(measure);
                                        if(progress.wasCanceled())
                                        {
                                            break;
                                        }
                                        measure++;
                                        MdfDatasetManager newDataset;
                                        OFCondition loadCond = newDataset.loadFile(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(),ERM_autoDetect,EXS_Unknown);
                                        if(loadCond==EC_Normal)
                                        {
                                            int count = 0;
                                            for(int modifyCounter = 0; modifyCounter<tagNames.size(); modifyCounter++)
                                            {
                                                QString fullTag = "(";
                                                fullTag.append(tagNames.at(modifyCounter).groupNumber);
                                                fullTag.append(",");
                                                fullTag.append(tagNames.at(modifyCounter).elementNumber);
                                                fullTag.append(")");
                                                OFCondition tempError = newDataset.modifyAllTags(fullTag.toLocal8Bit().constData(), tagNames.at(modifyCounter).value.toLocal8Bit().constData(), OFTrue, count, OFFalse);
                                                if(tempError != EC_Normal)
                                                {
                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), tempError.text(), selectedImages.at(0), fullTag.toLocal8Bit().constData());
                                                }
                                            }
                                            if(UIDChanges == true)
                                            {
                                                if(seriesInstanceListCF.size()==correctList.size())
                                                {
                                                    if(QString::compare(seriesInstanceListCF.at(selectedImages.at(0)).fileNames, correctList.at(selectedImages.at(0)), Qt::CaseInsensitive) == 0 &&
                                                            QString::compare(seriesInstanceListCF.at(selectedImages.at(0)).UID, QString("Error"), Qt::CaseInsensitive)!=0)
                                                    {
                                                        OFCondition seriesUIDErr = newDataset.modifyAllTags("(0020,000E)", seriesInstanceListCF.at(selectedImages.at(0)).UID.toLocal8Bit().constData(), OFTrue, count, OFFalse);
                                                        if(seriesUIDErr != EC_Normal)
                                                        {
                                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), seriesUIDErr.text(), 0, "(0020,000E)");
                                                        }
                                                    }
                                                    else
                                                    {
                                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Series UID", 0, "(0020,000E)");
                                                    }
                                                }
                                                else
                                                {
                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Series UID", 0, "(0020,000E)");
                                                }
                                                if(studyInstanceListCF.size()==correctList.size())
                                                {
                                                    if(QString::compare(studyInstanceListCF.at(selectedImages.at(0)).fileNames, correctList.at(selectedImages.at(0)), Qt::CaseInsensitive) == 0 &&
                                                            QString::compare(studyInstanceListCF.at(selectedImages.at(0)).UID, QString("Error"), Qt::CaseInsensitive)!=0)
                                                    {
                                                        OFCondition studyUIDErr = newDataset.modifyAllTags("(0020,000D)", studyInstanceListCF.at(selectedImages.at(0)).UID.toLocal8Bit().constData(), OFTrue, count, OFFalse);
                                                        if(studyUIDErr != EC_Normal)
                                                        {
                                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), studyUIDErr.text(), 0, "(0020,000D)");
                                                        }
                                                    }
                                                    else
                                                    {
                                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Study UID", 0, "(0020,000D)");
                                                    }

                                                }
                                                else
                                                {
                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Study UID", 0, "(0020,000E)");
                                                }
                                            }
                                            string destinationFilename;
                                            string hash="/";
                                            destinationFilename=destDirectory.path().toLocal8Bit().constData()+hash+correctList.at(selectedImages.at(0)).split("/").last().toLocal8Bit().constData();
                                            OFCondition saveError = newDataset.saveFile(destinationFilename.c_str()/*, xfer,EET_UndefinedLength,EGL_recalcGL,EPD_noChange,0,0,OFFalse*/);
                                            if(saveError != EC_Normal)
                                            {
                                                QString errorMessage(QString("Saving file, "));
                                                errorMessage.append(saveError.text());
                                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), errorMessage.toLocal8Bit().constData(), selectedImages.at(0), "No tag");
                                            }
                                            else if(saveError == EC_Normal)
                                            {
                                                finishedList<<destinationFilename.c_str();
                                                selectedImages.remove(0);
                                                ui->finishedList->clear();
                                                ui->finishedList->addItems(finishedList);
                                                ui->finishedList->show();
                                            }
                                        }
                                    }
                                    progress.setValue(listSize);
                                }
                                else
                                {
                                    tagModified = true;
                                    showBigImage();
                                }
                            }
                        }
                        else
                        {
                            QMessageBox::information(this, "Message!", "Desitnation folder not selected or could not be created! Please select an accessible destination folder!");
                        }
                    }
                    else if(execStatus == QDialog::Rejected)
                    {
                        tagModelStarted = false;
                    }
                }
                remove(txtFileName.toLocal8Bit().constData());
            }
            else
            {
                QStringList errorMessage;
                errorMessage.append("Loading file, ");
                errorMessage.append(sourceLoadFile.text());
                QString message = errorMessage.join("");
                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), message.toLocal8Bit().constData(), selectedImages.at(0), "No tag");
                fileformat.clear();
            }
            delete tagDialog;
        }
        else
        {
            QMessageBox::information(this, "Message", "No files selected or no file is there to be modified!");
        }
    }
    else
    {
        QMessageBox::information(this, "Message!", "No file uploaded yet!");
    }
}

void MainWindow::mediator()
{
    if(tagModelStarted == true)
    {
        connect(tagDialog->model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(dataChanged(QStandardItem*)));
    }
}

void MainWindow::dataChanged(QStandardItem * itm)
{
    if(changeList.size()==0)
    {
        changeList.append(itm->index().row());
    }
    else
    {
        bool changed = true;
        for(int i = 0; i<changeList.size(); i++)
        {
            if(changeList.at(i)==itm->index().row())
            {
                changed = false;
            }
        }
        if(changed == true)
        {
            changeList.append(itm->index().row());
        }
    }
}
void MainWindow::refresher()
{
    if(inModifyFunc == false)
    {
        int i = ui->bigImageLabel->bandList.size();
        while(i>0)
        {
            ui->bigImageLabel->bandList.at(i-1)->hide();
            ui->bigImageLabel->bandList.at(i-1)->setGeometry(QRect(0,0,0,0));
            ui->bigImageLabel->bandList.at(i-1)->exists = false;
            delete ui->bigImageLabel->bandList.at(i-1);
            ui->bigImageLabel->bandList.remove(i-1);
            ui->bigImageLabel->bandList.squeeze();
            i--;
        }
        ui->boxes->setText(QString::number(ui->bigImageLabel->bandList.size()));
    }
}

void MainWindow::closeError()
{
    errorLogOpen = false;
    ui->errorEdit->hide();
    ui->errorClose->hide();
    ui->clearError->hide();
}

void MainWindow::showError()
{
    errorLogOpen = true;
    ui->errorEdit->setReadOnly(true);
    ui->errorEdit->show();
    ui->errorClose->show();
    ui->clearError->show();
}

void MainWindow::selectAllImage()
{
    selectedImages.clear();
    selectedList.clear();
    duplicateList.clear();
    counter = 0;
    ui->dontShowError->setChecked(false);
    ui->progressBar->setValue(counter);
    if(correctList.size()>0)
    {
        for(int i = 0; i<correctList.size(); i++)
        {
            selectedImages.append(i);
            duplicateList.append(i);
            selectedList<<correctList.at(selectedImages.at(selectedImages.size()-1));
        }
        ui->listOfImages->selectAll();
        ui->listOfImages->show();
        ui->selectedFiles->clear();
        ui->selectedFiles->addItems(selectedList);
        ui->selectedFiles->show();
    }
    else
    {
        QMessageBox::information(this, "Message","No image to select!");
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent *ev)
{
    if(QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) == false)
    {
        if(obj == ui->listOfImages->viewport())
        {
            if(ev->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent * mouseEv = static_cast<QMouseEvent*>(ev);
                if((mouseEv->buttons() & Qt::LeftButton) && (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == true)/* && (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) == false)*/)
                {
                    controlButtonCounter++;
                    fetch = true;
                }
                else if((mouseEv->buttons() & Qt::LeftButton) && (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == false)/* && (QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) == false)*/)
                {
                    if(selectedImages.size()>0)
                    {
                        ui->listOfImages->clearSelection();
                        selectedImages.clear();
                        selectedList.clear();
                        ui->selectedFiles->clear();
                        ui->selectedFiles->show();
                    }
                    fetch = false;
                    controlButtonCounter = 0;
                }
            }
        }
        if(obj == ui->listOfImages)
        {
            if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == false)
            {
                if(ev->type() == QEvent::KeyPress)
                {
                    QKeyEvent * keyEv = static_cast<QKeyEvent*>(ev);
                    if(keyEv->key() == Qt::Key_Up)
                    {
                        if(selectedImages.size()>0)
                        {
                            selectedImages.clear();
                            selectedList.clear();
                            ui->selectedFiles->clear();
                            ui->selectedFiles->show();
                        }
                        serial = ui->listOfImages->selectionModel()->currentIndex().row();
                        if(serial>0)
                        {
                            serial = serial - 1;
                        }
                        if(serial-1<correctList.size())
                        {
                            if(selectedImages.size()==0)
                            {
                                selectedImages.append(serial);
                                duplicateList.append(serial);
                                selectedList<<correctList.at(selectedImages.at(0));
                                ui->selectedFiles->clear();
                                ui->selectedFiles->addItems(selectedList);
                                ui->selectedFiles->show();
                            }
                            regionValues(selectedImages.at(0));
                        }
                    }
                    else if(keyEv->key() == Qt::Key_Down)
                    {
                        if(selectedImages.size()>0)
                        {
                            selectedImages.clear();
                            selectedList.clear();
                            ui->selectedFiles->clear();
                            ui->selectedFiles->show();
                        }
                        serial = ui->listOfImages->selectionModel()->currentIndex().row();
                        if(serial<correctList.size()-1)
                        {
                            serial = serial + 1;
                        }
                        if(serial-1<correctList.size())
                        {
                            if(selectedImages.size()==0)
                            {
                                selectedImages.append(serial);
                                duplicateList.append(serial);
                                selectedList<<correctList.at(selectedImages.at(0));
                                ui->selectedFiles->clear();
                                ui->selectedFiles->addItems(selectedList);
                                ui->selectedFiles->show();
                            }
                            regionValues(selectedImages.at(0));
                        }
                    }
                }
            }
            else if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == true)
            {
                return true;
            }
        }
        if(obj == ui->selectedFiles)
        {
            if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == false)
            {
                if(ev->type() == QEvent::KeyPress)
                {
                    QKeyEvent * keyTableEv = static_cast<QKeyEvent*>(ev);
                    if(keyTableEv->key() == Qt::Key_Up)
                    {
                        int index = ui->selectedFiles->selectionModel()->currentIndex().row();
                        if(index>0)
                        {
                            index = index - 1;
                        }
                        if(inModifyFunc == false && bigImageShown == true)
                        {
                            if(duplicateList.size()>0)
                            {
                                regionValues(duplicateList.at(index));
                            }
                        }
                    }
                    else if(keyTableEv->key() == Qt::Key_Down)
                    {
                        int index = ui->selectedFiles->selectionModel()->currentIndex().row();
                        if( index < duplicateList.size()-1)
                        {
                            index = index + 1;
                        }
                        if(inModifyFunc == false && bigImageShown == true)
                        {
                            if(duplicateList.size()>0)
                            {
                                regionValues(duplicateList.at(index));
                            }
                        }
                    }
                }
            }
            else if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == true)
            {
                return true;
            }
        }
    }
    else if(QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) == true)
    {
        return true;
    }
    return QObject::eventFilter(obj,ev);
}

void MainWindow::clearFiles()
{
    if(stringList.size()>0)
    {
        if(correctList.size()>0)
        {
            model->clear();
            ui->listOfImages->reset();
            ui->listOfImages->setModel(model);
            ui->listOfImages->show();
            delete model;
        }
        if(defectedPixelData.size()>0)
        {
            defectedModel->clear();
            ui->listOfdefectedPixelData->reset();
            ui->listOfdefectedPixelData->setModel(defectedModel);
            ui->listOfdefectedPixelData->show();
            delete defectedModel;
        }
        tagNames.clear();
        taskIndicator = 0;
        inModifyFunc = false;
        firstTime = true;
        tableCounter = 0;
        errorCounter = 0;
        counter = 0;
        ui->WrongList->clear();
        ui->finishedList->clear();
        defectedPixelData.clear();
        stringList.clear();
        correctList.clear();
        finishedList.clear();
        wrongList.clear();
        ui->label->clear();
        ui->label->show();
        selectedImages.clear();
        selectedList.clear();
        ui->selectedFiles->clear();
        refresher();
        ui->bigImageLabel->clear();
        images.clear();
        if(deleteImage == true)
        {
            delete image;
            deleteImage = false;
        }
    }
}

void MainWindow::IDListFillup()
{
    if(correctList.size())
    {
        QVector<IDList>studyInstanceListCFTemp;
        QVector<IDList>seriesInstanceListCFTemp;
        QStringList studyInstanceTempCF;
        QStringList seriesInstanceTempCF;
        for(int i=0; i<correctList.size(); i++)
        {
            DcmFileFormat file;
            OFCondition loadCond = file.loadFile(correctList.at(i).toLocal8Bit().constData());
            if(loadCond == EC_Normal)
            {
                DcmDataset * dset = file.getDataset();
                OFString studyInstanceUID;
                OFCondition stIUIDErr = dset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID, 0, true);
                if(stIUIDErr == EC_Normal)
                {
                    studyInstanceListCFTemp.append(IDList(correctList.at(i), QString(studyInstanceUID.c_str())));
                    studyInstanceTempCF.append(QString(studyInstanceUID.c_str()));
                }
                else
                {
                    studyInstanceListCFTemp.append(IDList(correctList.at(i), QString("Error")));
                }
                OFString seriesInstanceUID;
                OFCondition srIUIDErr = dset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID, 0, true);
                if(srIUIDErr == EC_Normal)
                {
                    seriesInstanceListCFTemp.append(IDList(correctList.at(i), QString(seriesInstanceUID.c_str())));
                    seriesInstanceTempCF.append(QString(seriesInstanceUID.c_str()));
                }
                else
                {
                    seriesInstanceListCFTemp.append(IDList(correctList.at(i), QString("Error")));
                }
                dset->clear();
            }
            else
            {
                studyInstanceListCFTemp.append(IDList(correctList.at(i), QString("Error")));
                seriesInstanceListCFTemp.append(IDList(correctList.at(i), QString("Error")));
            }
            file.clear();
        }
        QVector<UIDTempHolder> studyInstanceCFreplacer;
        QSet<QString> studyInstanceSetCF = QSet<QString>::fromList(studyInstanceTempCF);
        for( QSet<QString>::ConstIterator jj = studyInstanceSetCF.begin(); jj != studyInstanceSetCF.end(); ++jj )
        {
            char uid[100];
            dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
            studyInstanceCFreplacer.append(UIDTempHolder(*jj, QString(uid)));
        }
        QVector<UIDTempHolder> seriesInstanceCFreplacer;
        QSet<QString> seriesInstanceSetCF = QSet<QString>::fromList(seriesInstanceTempCF);
        for( QSet<QString>::ConstIterator kk = seriesInstanceSetCF.begin(); kk != seriesInstanceSetCF.end(); ++kk )
        {
            char uid[100];
            dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
            seriesInstanceCFreplacer.append(UIDTempHolder(*kk, QString(uid)));
        }
        studyInstanceListCF.clear();

        for(int secondCounter=0; secondCounter<studyInstanceListCFTemp.size(); secondCounter++)
        {
            if(strcmp("Error", studyInstanceListCFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
            {
                studyInstanceListCF.append(studyInstanceListCFTemp.at(secondCounter));
            }
            else
            {
                for(int firstCounter=0; firstCounter<studyInstanceCFreplacer.size(); firstCounter++)
                {
                    if(strcmp(studyInstanceCFreplacer.at(firstCounter).originalValue.toLocal8Bit().constData(), studyInstanceListCFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
                    {
                        studyInstanceListCF.append(IDList(studyInstanceListCFTemp.at(secondCounter).fileNames, studyInstanceCFreplacer.at(firstCounter).newValue));
                    }
                }
            }
        }
        seriesInstanceListCF.clear();
        for(int secondCounter=0; secondCounter<seriesInstanceListCFTemp.size(); secondCounter++)
        {
            if(strcmp("Error", seriesInstanceListCFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
            {
                seriesInstanceListCF.append(seriesInstanceListCFTemp.at(secondCounter));
            }
            else
            {
                for(int firstCounter=0; firstCounter<seriesInstanceCFreplacer.size(); firstCounter++)
                {
                    if(strcmp(seriesInstanceCFreplacer.at(firstCounter).originalValue.toLocal8Bit().constData(), seriesInstanceListCFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
                    {
                        seriesInstanceListCF.append(IDList(seriesInstanceListCFTemp.at(secondCounter).fileNames, seriesInstanceCFreplacer.at(firstCounter).newValue));
                    }
                }
            }
        }
    }
    if(defectedPixelData.size())
    {
        QVector<IDList>studyInstanceListDFTemp;
        QVector<IDList>seriesInstanceListDFTemp;
        QStringList studyInstanceTempDF;
        QStringList seriesInstanceTempDF;
        for(int i=0; i<defectedPixelData.size(); i++)
        {
            DcmFileFormat file;
            OFCondition loadCond = file.loadFile(defectedPixelData.at(i).toLocal8Bit().constData());
            if(loadCond == EC_Normal)
            {
                DcmDataset * dset = file.getDataset();
                OFString studyInstanceUID;
                OFCondition stIUIDErr = dset->findAndGetOFString(DCM_StudyInstanceUID, studyInstanceUID, 0, true);
                if(stIUIDErr == EC_Normal)
                {
                    studyInstanceListDFTemp.append(IDList(defectedPixelData.at(i), QString(studyInstanceUID.c_str())));
                    studyInstanceTempDF.append(QString(studyInstanceUID.c_str()));
                }
                else
                {
                    studyInstanceListDFTemp.append(IDList(defectedPixelData.at(i), QString("Error")));
                }
                OFString seriesInstanceUID;
                OFCondition srIUIDErr = dset->findAndGetOFString(DCM_SeriesInstanceUID, seriesInstanceUID, 0, true);
                if(srIUIDErr == EC_Normal)
                {
                    seriesInstanceListDFTemp.append(IDList(defectedPixelData.at(i), QString(seriesInstanceUID.c_str())));
                    seriesInstanceTempDF.append(QString(seriesInstanceUID.c_str()));
                }
                else
                {
                    seriesInstanceListDFTemp.append(IDList(defectedPixelData.at(i), QString("Error")));
                }
                dset->clear();
            }
            else
            {
                studyInstanceListDFTemp.append(IDList(defectedPixelData.at(i), QString("Error")));
                seriesInstanceListDFTemp.append(IDList(defectedPixelData.at(i), QString("Error")));
            }
            file.clear();
        }
        QVector<UIDTempHolder> studyInstanceDFreplacer;
        QSet<QString> studyInstanceSetDF = QSet<QString>::fromList(studyInstanceTempDF);
        for( QSet<QString>::ConstIterator jj = studyInstanceSetDF.begin(); jj != studyInstanceSetDF.end(); ++jj )
        {
            char uid[100];
            dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
            studyInstanceDFreplacer.append(UIDTempHolder(*jj, QString(uid)));
        }
        QVector<UIDTempHolder> seriesInstanceDFreplacer;
        QSet<QString> seriesInstanceSetDF = QSet<QString>::fromList(seriesInstanceTempDF);
        for( QSet<QString>::ConstIterator kk = seriesInstanceSetDF.begin(); kk != seriesInstanceSetDF.end(); ++kk )
        {
            char uid[100];
            dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
            seriesInstanceDFreplacer.append(UIDTempHolder(*kk, QString(uid)));
        }
        studyInstanceListDF.clear();
        for(int secondCounter=0; secondCounter<studyInstanceListDFTemp.size(); secondCounter++)
        {
            if(strcmp("Error", studyInstanceListDFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
            {
                studyInstanceListDF.append(studyInstanceListDFTemp.at(secondCounter));
            }
            else
            {
                for(int firstCounter=0; firstCounter<studyInstanceDFreplacer.size(); firstCounter++)
                {
                    if(strcmp(studyInstanceDFreplacer.at(firstCounter).originalValue.toLocal8Bit().constData(), studyInstanceListDFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
                    {
                        studyInstanceListDF.append(IDList(studyInstanceListDFTemp.at(secondCounter).fileNames, studyInstanceDFreplacer.at(firstCounter).newValue));
                    }
                }
            }
        }
        seriesInstanceListDF.clear();
        for(int secondCounter=0; secondCounter<seriesInstanceListDFTemp.size(); secondCounter++)
        {
            if(strcmp("Error", seriesInstanceListDFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
            {
                seriesInstanceListDF.append(seriesInstanceListDFTemp.at(secondCounter));
            }
            else
            {
                for(int firstCounter=0; firstCounter<seriesInstanceDFreplacer.size(); firstCounter++)
                {
                    if(strcmp(seriesInstanceDFreplacer.at(firstCounter).originalValue.toLocal8Bit().constData(), seriesInstanceListDFTemp.at(secondCounter).UID.toLocal8Bit().constData()) == 0)
                    {
                        seriesInstanceListDF.append(IDList(seriesInstanceListDFTemp.at(secondCounter).fileNames, seriesInstanceDFreplacer.at(firstCounter).newValue));
                    }
                }
            }
        }
    }
}

void MainWindow::endingMsg()
{
    ui->progressBar->setValue(counter);
    if(overrideCursor == true)
    {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        enableInputs();
        overrideCursor = false;
    }
    QMessageBox::information(this, "Message", "Task finished");
    //checkError = true;
    ui->listOfImages->clearSelection();
    ui->selectedFiles->clear();
    selectedList.clear();
    ui->dontShowError->setChecked(false);
    counter = 0;
    inModifyFunc = false;
    destinationSelected = false;
    closeBigImage();
    ui->progressBar->reset();
    ui->progressBar->setValue(0);
    if(corruptedFiles.size()>0)
    {
        QVector<int>temp;
        int j2 = -1;
        int k=0;
        while(k<corruptedFiles.size())
        {
            int j1 = correctList.size();
            for(int i = 0; i<corruptedFiles.size(); i++)
            {
                if(corruptedFiles.at(i) <= j1 && corruptedFiles.at(i)>j2)
                {
                    j1 = corruptedFiles.at(i);
                }
                if(i == corruptedFiles.size()-1)
                {
                    temp.append(j1);
                    j2 = j1;
                    k++;
                }
            }
        }
        for(int i=temp.size(); i>0; i--)
        {
            correctList.removeAt(temp.at(i-1));
            images.remove(temp.at(i-1));
            imageBoundaries.remove(temp.at(i-1));
        }
        if(correctList.size()==0)
        {
            ui->label->clear();
            ui->label->hide();
        }

        int rowPointer = 0;
        model->clear();
        for(int i=0; i<correctList.size(); i++)
        {
            rowPointer = i;
            QPixmap pMap = QPixmap::fromImage(images.at(i), Qt::AutoColor);
            QStandardItem * itm = new QStandardItem;
            itm->setIcon(pMap);
            QString n = correctList.at(i);
            itm->setText(n);
            itm->setFlags(itm->flags() ^ Qt::ItemIsEditable);
            itm->setTextAlignment(Qt::AlignBottom);
            model->setItem(rowPointer, 0, itm);
        }
        ui->listOfImages->setModel(model);
        ui->listOfImages->show();
        ui->listOfImages->setStyleSheet(QString("icon-size: 150px 150px"));
        ui->listOfImages->verticalHeader()->setVisible(false);
        ui->listOfImages->horizontalHeader()->setVisible(false);
        for(int i=0; i<=rowPointer; i++)
        {
            ui->listOfImages->setRowHeight(i,100);
            ui->listOfImages->setColumnWidth(i,300);
        }
        corruptedFiles.clear();
    }
    if(correctList.size()==0)
    {
        delete model;
    }
    refresher();
    ui->bigImageLabel->clear();
    tagNames.clear();
    taskIndicator = 0;
    tableCounter = images.size();
}

void MainWindow::destinationDirectory()
{
    QFileDialog * destFolder = new QFileDialog(this, Qt::Widget);
    destFolder->setWindowModality(Qt::ApplicationModal);
    QString destFiles = destFolder->getExistingDirectory(this,tr("Directory"),destDirectory.path());
    if(destFiles.length()>1)
    {
        QDir dir(destFiles);
        if(dir.exists())
        {
            string hash="/Modified files";
            if(strcmp(destFiles.toLocal8Bit().constData()+destFiles.length()-1,"/")==0)
            {
                hash = "Modified files";
            }
            string oldDir = destFiles.toLocal8Bit().constData();
            string newDir = oldDir+hash;
            if(dir.exists(newDir.c_str())==false)
            {
                if(dir.mkdir(newDir.c_str()))
                {
                    destDirectory.setPath(newDir.c_str());
                    destinationSelected = true;
                }
                else
                {
                    QMessageBox::information(this, "Message!", "No permission to write in the desired directory!");
                    destinationSelected = false;
                }
            }
            else
            {
                destDirectory.setPath(newDir.c_str());
                destinationSelected = true;
            }
        }
        else
        {
            QMessageBox::information(this, "Message!", "Either the selected path doesn't exist or the folder got deleted! \r\n Please re-select a new desination folder");
            destinationSelected = false;
        }
    }
    delete destFolder;
}

void MainWindow::showTheSelectedList()
{
    QFileDialog * fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    QListView* list = fileDialog->findChild<QListView*>("listView");
    if(list)
    {
        list->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    QTreeView* tree = fileDialog->findChild<QTreeView*>();
    if(tree)
    {
        tree->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    if(fileDialog->exec())
    {
        if(fileDialog->selectedFiles().size()>0)
        {
            destinationSelected = false;
            stringList.clear();
            stringList = fileDialog->selectedFiles();
            if(ui->prevFileCheck->isChecked() == false || firstTime == true)
            {
                ui->label->clear();
                correctList.clear();
                wrongList.clear();
                ui->WrongList->clear();
                finishedList.clear();
                images.clear();
                selectedImages.clear();
                selectedList.clear();
                ui->selectedFiles->clear();
                ui->finishedList->clear();
                defectedPixelData.clear();
                imageBoundaries.clear();
                if(firstTime == false)
                {
                    model->clear();
                    ui->listOfImages->reset();
                    ui->listOfImages->setModel(model);
                    defectedModel->clear();
                    ui->listOfdefectedPixelData->reset();
                    ui->listOfdefectedPixelData->setModel(defectedModel);
                }
                ui->listOfdefectedPixelData->reset();
                model = new QStandardItemModel(1, 1, this);
                tableCounter = 0;
                defectedModel = new QStandardItemModel(1, 1, this);
                errorCounter = 0;
                firstTime = false;
            }
            DcmFileFormat fileformat;
            int listsize=stringList.size();
            QProgressDialog progress("Loading files...", "Cancel", 0, listsize, this);
            progress.setWindowTitle("Progress");
            progress.setWindowModality(Qt::WindowModal);
            progress.setWindowFlags(Qt::CustomizeWindowHint  | Qt::WindowTitleHint | Qt::Window);
            for(int listCounter=0; listCounter<listsize; listCounter++)
            {
                progress.setValue(listCounter);
                if(progress.wasCanceled())
                {
                    break;
                }
                string filename=stringList.at(listCounter).toLocal8Bit().constData();
                OFCondition loadFileError = fileformat.loadFile(filename.c_str());
                if(EC_Normal == loadFileError)
                {
                    string modalInd = "";
                    DcmDataset* dataset = fileformat.getDataset();
                    E_TransferSyntax xfer= dataset->getOriginalXfer();
                    bool pixelDataCheck = true;
                    bool DataCheck = dataset->tagExists(DCM_PixelData, true);
                    if(true == DataCheck)
                    {
                        OFCondition rowCheck=dataset->findAndGetUint16(DCM_Rows, rows);
                        if(EC_Normal == rowCheck)
                        {
                            OFCondition colCheck=dataset->findAndGetUint16(DCM_Columns, cols);
                            if(EC_Normal == colCheck)
                            {
                                DJDecoderRegistration::registerCodecs();
                                long int numberOfFrames = 0;
                                Uint16 PlanCon;
                                bool frameOK = true;
                                if(dataset->tagExists(DCM_NumberOfFrames))
                                {
                                    OFCondition NOF = dataset->findAndGetLongInt(DCM_NumberOfFrames, numberOfFrames);
                                    if(EC_Normal != NOF)
                                    {
                                        frameOK = false;
                                        pixelDataCheck = false;
                                        QStringList errorMessage;
                                        errorMessage.append("Frames in image, ");
                                        errorMessage.append(NOF.text());
                                        QString message = errorMessage.join("");
                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0008)");
                                    }
                                }
                                else
                                {
                                    numberOfFrames = 1;
                                }
                                if(frameOK == true)
                                {

                                    OFString modality;
                                    OFCondition getModalErr = dataset->findAndGetOFString(DCM_Modality, modality, 0, OFTrue);
                                    if(getModalErr == EC_Normal)
                                    {
                                        if(strcmpi(modality.c_str(), "MR")!=0 && strcmpi(modality.c_str(), "CT")!=0)
                                        {
                                            const char* photoMetInter;
                                            OFCondition photometInt = dataset->findAndGetString(DCM_PhotometricInterpretation, photoMetInter, true);
                                            if(photometInt == EC_Normal)
                                            {
                                                DcmElement * element = NULL;
                                                OFCondition elemError = dataset->findAndGetElement(DCM_PixelData, element);
                                                if(EC_Normal == elemError)
                                                {
                                                    Uint32 singleFrameSize = 0;
                                                    OFCondition UCSizeE = element->getUncompressedFrameSize(dataset, singleFrameSize);
                                                    if(EC_Normal == UCSizeE)
                                                    {
                                                        Uint16 samplesPerPixel;
                                                        OFCondition sPP = dataset->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
                                                        if(sPP == EC_Normal)
                                                        {
                                                            bool alright = false;
                                                            if((int)samplesPerPixel == 3)
                                                            {
                                                                QImage imageData(cols, rows, QImage::Format_RGB888);
                                                                Uint16 bitsAllocated;
                                                                OFCondition bitsAlloErr = dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
                                                                if(bitsAlloErr == EC_Normal)
                                                                {
                                                                    if((int)bitsAllocated == 8)
                                                                    {
                                                                        OFCondition PlanarConf = dataset->findAndGetUint16(DCM_PlanarConfiguration, PlanCon);
                                                                        if(PlanarConf == EC_Normal)
                                                                        {
                                                                            if((int)PlanCon == 1)
                                                                            {
                                                                                const Uint8* fullBuffer = 0;
                                                                                int totalSize = (int)singleFrameSize*(int)numberOfFrames;
                                                                                Uint8 * tempBuffer = new Uint8[totalSize];
                                                                                Uint8 * temp = new Uint8[int(singleFrameSize)];
                                                                                unsigned long * count = 0;
                                                                                OFCondition getImgDat = dataset->findAndGetUint8Array(DCM_PixelData, fullBuffer, count, OFFalse);
                                                                                if(getImgDat == EC_Normal)
                                                                                {
                                                                                    if(fullBuffer!=NULL)
                                                                                    {
                                                                                        int numPix = totalSize/(int)samplesPerPixel;
                                                                                        for(int n = 0; n<numPix; n++)
                                                                                        {
                                                                                            for(int s=0; s<samplesPerPixel; s++)
                                                                                            {
                                                                                                tempBuffer[n*(int)samplesPerPixel+s] = fullBuffer[n+(int)numPix*s];
                                                                                            }
                                                                                        }
                                                                                        if(strncmp(photoMetInter, "YBR_FULL", strlen("YBR_FULL")) == 0)
                                                                                        {
                                                                                            int index = 0;
                                                                                            memcpy(temp,tempBuffer,singleFrameSize);
                                                                                            Uint8* newTemp = new Uint8[int(singleFrameSize)];
                                                                                            for(unsigned long y = (unsigned long)0; y < (unsigned long)rows ; y++)
                                                                                            {
                                                                                                for(unsigned long x = (unsigned long)0; x < (unsigned long)cols; x++)
                                                                                                {
                                                                                                    index=((int)x + ((int)y*(int)cols))*(int)samplesPerPixel;
                                                                                                    if(index<int(singleFrameSize)-2)
                                                                                                    {
                                                                                                        const double Y  = (int)temp[index];
                                                                                                        const double Cb = (int)temp[index+1];
                                                                                                        const double Cr = (int)temp[index+2];
                                                                                                        const double r = Y + 1.402 * (Cr-128);
                                                                                                        const double g = Y - 0.344136 * (Cb-128) - 0.714136 * (Cr-128);
                                                                                                        const double b = Y + 1.772   * (Cb-128);
                                                                                                        double R = r < 0 ? 0 : r;
                                                                                                        R = R > 255 ? 255 : R;
                                                                                                        double G = g < 0 ? 0 : g;
                                                                                                        G = G > 255 ? 255 : G;
                                                                                                        double B = b < 0 ? 0 : b;
                                                                                                        B = B > 255 ? 255 : B;
                                                                                                        assert( R >= 0 && R <= 255 );
                                                                                                        assert( G >= 0 && G <= 255 );
                                                                                                        assert( B >= 0 && B <= 255 );
                                                                                                        newTemp[index] = (Uint8)((R < 0 ? 0 : R) > 255 ? 255 : R);
                                                                                                        newTemp[index+1] = (Uint8)G;
                                                                                                        newTemp[index+2] = (Uint8)B;
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                            memcpy(imageData.bits(), newTemp, singleFrameSize);
                                                                                            images.append(imageData);
                                                                                            alright = true;
                                                                                            delete [] temp;
                                                                                            delete [] newTemp;
                                                                                            delete [] tempBuffer;
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            memcpy(imageData.bits(),tempBuffer,singleFrameSize);
                                                                                            images.append(imageData);
                                                                                            alright = true;
                                                                                            delete [] temp;
                                                                                            delete [] tempBuffer;
                                                                                        }
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        delete [] temp;
                                                                                        delete [] tempBuffer;
                                                                                        pixelDataCheck = false;
                                                                                        QString message = "Pixel data, memory access error!";
                                                                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    delete [] temp;
                                                                                    delete [] tempBuffer;
                                                                                    pixelDataCheck = false;
                                                                                    QStringList errorMessage;
                                                                                    errorMessage.append("Pixel data, ");
                                                                                    errorMessage.append(getImgDat.text());
                                                                                    QString message = errorMessage.join("");
                                                                                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                OFString decompressedColorModel = NULL;
                                                                                OFCondition gDcCME = element->getDecompressedColorModel(dataset, decompressedColorModel);
                                                                                if(EC_Normal == gDcCME)
                                                                                {
                                                                                    bool OffsetCheck = true;
                                                                                    for(int tFcount=0; tFcount<numberOfFrames; tFcount++)
                                                                                    {
                                                                                        Uint8 * tBuffer = new Uint8[int(singleFrameSize)];//Buffer for each frame
                                                                                        DcmFileCache * cache=NULL;
                                                                                        Uint32 startFragment = 0;
                                                                                        OFCondition UFrame=element->getUncompressedFrame(dataset,tFcount,startFragment,tBuffer,int(singleFrameSize),decompressedColorModel,cache);
                                                                                        if(EC_Normal!=UFrame)
                                                                                        {
                                                                                            OffsetCheck = false;
                                                                                            pixelDataCheck = false;
                                                                                            QString message = UFrame.text();
                                                                                            errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                                                                                            delete [] tBuffer;
                                                                                            break;
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            delete [] tBuffer;
                                                                                        }
                                                                                    }
                                                                                    if(OffsetCheck == true)
                                                                                    {
                                                                                        Uint8 * temp = new Uint8[int(singleFrameSize)];
                                                                                        DcmFileCache * cache = NULL;
                                                                                        Uint32 startFragment = 0;
                                                                                        OFCondition getImgE = element->getUncompressedFrame(dataset, 0, startFragment, temp, singleFrameSize, decompressedColorModel, cache);
                                                                                        if(EC_Normal == getImgE)
                                                                                        {
                                                                                            if(temp!=NULL)
                                                                                            {
                                                                                                memcpy(imageData.bits(), temp, singleFrameSize);
                                                                                                images.append(imageData);
                                                                                                alright = true;
                                                                                                delete [] temp;
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                delete [] temp;
                                                                                                pixelDataCheck = false;
                                                                                                QString message = "Pixel data, Memory access error!";
                                                                                                errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                                                                                            }
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            delete [] temp;
                                                                                            pixelDataCheck = false;
                                                                                            QStringList errorMessage;
                                                                                            errorMessage.append("Getting uncompressed frame, ");
                                                                                            errorMessage.append(getImgE.text());
                                                                                            QString message = errorMessage.join("");
                                                                                            errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                                                                                        }
                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    pixelDataCheck = false;
                                                                                    QStringList errorMessage;
                                                                                    errorMessage.append("Decompressed color model, ");
                                                                                    errorMessage.append(gDcCME.text());
                                                                                    QString message = errorMessage.join("");
                                                                                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            pixelDataCheck = false;
                                                                            QStringList errorMessage;
                                                                            errorMessage.append("Planar configuration, ");
                                                                            errorMessage.append(PlanarConf.text());
                                                                            QString message = errorMessage.join("");
                                                                            errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0006)");
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        pixelDataCheck = false;
                                                                        QStringList errorMessage;
                                                                        errorMessage.append("RGB image with only 8 bit is supported.");
                                                                        QString message = errorMessage.join("");
                                                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0100)");
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    pixelDataCheck = false;
                                                                    QStringList errorMessage;
                                                                    errorMessage.append("Bits Allocated, ");
                                                                    errorMessage.append(bitsAlloErr.text());
                                                                    QString message = errorMessage.join("");
                                                                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0100)");
                                                                }
                                                            }
                                                            else if(samplesPerPixel == 1)
                                                            {
                                                                if(strcmpi(photoMetInter, "MONOCHROME2") == 0)
                                                                {
                                                                    OFString decompressedColorModel = NULL;
                                                                    OFCondition gDcCME = element->getDecompressedColorModel(dataset, decompressedColorModel);
                                                                    if(EC_Normal == gDcCME)
                                                                    {
                                                                        Uint16 bitsAllocated;
                                                                        OFCondition bitsAlloErr = dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
                                                                        if(bitsAlloErr == EC_Normal)
                                                                        {
                                                                            if((int)bitsAllocated == 8)
                                                                            {
                                                                                Uint8 * temp = new Uint8[int(singleFrameSize)];
                                                                                DcmFileCache * cache = NULL;
                                                                                Uint32 startFragment = 0;
                                                                                OFCondition getImgE = element->getUncompressedFrame(dataset, 0, startFragment, temp, singleFrameSize, decompressedColorModel, cache);
                                                                                if(EC_Normal == getImgE)
                                                                                {
                                                                                    QImage imageData(cols, rows, QImage::Format_Indexed8);
                                                                                    memcpy(imageData.bits(),temp,singleFrameSize);
                                                                                    images.append(imageData);
                                                                                    alright = true;
                                                                                    delete [] temp;
                                                                                }
                                                                                else
                                                                                {
                                                                                    pixelDataCheck = false;
                                                                                    delete [] temp;
                                                                                    QStringList errorMessage;
                                                                                    errorMessage.append("Getting uncompressed frame, ");
                                                                                    errorMessage.append(getImgE.text());
                                                                                    QString message = errorMessage.join("");
                                                                                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "No tag");
                                                                                }
                                                                            }
                                                                            else if((int)bitsAllocated == 16)
                                                                            {
                                                                                Uint16 bitStored;
                                                                                OFCondition bitsStorErr = dataset->findAndGetUint16(DCM_BitsStored, bitStored);
                                                                                if(bitsStorErr == EC_Normal)
                                                                                {
                                                                                    Uint16 * temp = new Uint16[int(singleFrameSize)];
                                                                                    Uint8 * tempBuffer = new Uint8[int(singleFrameSize)];
                                                                                    DcmFileCache * cache = NULL;
                                                                                    Uint32 startFragment = 0;
                                                                                    OFCondition getImgE = element->getUncompressedFrame(dataset, 0, startFragment, temp, singleFrameSize, decompressedColorModel, cache);
                                                                                    if(EC_Normal == getImgE)
                                                                                    {
                                                                                        Uint16 min = temp[0];
                                                                                        for(int Aj = 0; Aj < cols*rows ; Aj++)
                                                                                        {
                                                                                            min = min<=temp[Aj]?min:temp[Aj];
                                                                                        }
                                                                                        Uint16 max = temp[0];
                                                                                        for(int Bj = 0; Bj < cols*rows ; Bj++)
                                                                                        {
                                                                                            max = max>=temp[Bj]?max:temp[Bj];
                                                                                        }
                                                                                        for(int Cj = 0; Cj < cols*rows ; Cj++)
                                                                                        {
                                                                                            double a = (double)temp[Cj];
                                                                                            double b = (double) min;
                                                                                            double c = (double) max;
                                                                                            double result = (a-b)/(c-b);
                                                                                            tempBuffer[Cj]=(Uint8)255*result;
                                                                                        }
                                                                                        QImage imageData(cols, rows, QImage::Format_Indexed8);
                                                                                        memcpy(imageData.bits(),tempBuffer,cols*rows);
                                                                                        images.append(imageData);
                                                                                        alright = true;
                                                                                        delete [] temp;
                                                                                        delete [] tempBuffer;
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        pixelDataCheck = false;
                                                                                        delete [] temp;
                                                                                        delete [] tempBuffer;
                                                                                        QStringList errorMessage;
                                                                                        errorMessage.append("Getting uncompressed frame, ");
                                                                                        errorMessage.append(getImgE.text());
                                                                                        QString message = errorMessage.join("");
                                                                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "No tag");
                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    pixelDataCheck = false;
                                                                                    QStringList errorMessage;
                                                                                    errorMessage.append("Bits stored, ");
                                                                                    errorMessage.append(bitsStorErr.text());
                                                                                    QString message = errorMessage.join("");
                                                                                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0101)");
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                pixelDataCheck = false;
                                                                                QStringList errorMessage;
                                                                                errorMessage.append("RGB with 8bit or MONOCHROME2 image with only ");
                                                                                errorMessage.append("8 or 16 bit allocation is supported yet.");
                                                                                QString message = errorMessage.join("");
                                                                                errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0100)");
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            pixelDataCheck = false;
                                                                            QStringList errorMessage;
                                                                            errorMessage.append("Bits Allocated, ");
                                                                            errorMessage.append(bitsAlloErr.text());
                                                                            QString message = errorMessage.join("");
                                                                            errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0100)");
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        pixelDataCheck = false;
                                                                        QStringList errorMessage;
                                                                        errorMessage.append("Decompressed color model, ");
                                                                        errorMessage.append(gDcCME.text());
                                                                        QString message = errorMessage.join("");
                                                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "No tag");
                                                                    }
                                                                }
                                                                else if(strcmpi(photoMetInter, "PALETTE COLOR") == 0 || strcmpi(photoMetInter, "MONOCHROME1") == 0 )
                                                                {
                                                                    DicomImage paletteImage(dataset, dataset->getOriginalXfer());
                                                                    int imageCreated = paletteImage.writeBMP("PaletteImage.bmp", 0, 0);
                                                                    if(imageCreated)
                                                                    {
                                                                        QImage imageData(cols, rows, QImage::Format_Indexed8);
                                                                        QPixmap* tempImage = new QPixmap("PaletteImage.bmp");
                                                                        imageData = tempImage->toImage();
                                                                        images.append(imageData);
                                                                        alright = true;
                                                                        QFile::remove("PaletteImage.bmp");
                                                                        delete tempImage;
                                                                    }
                                                                    else
                                                                    {
                                                                        pixelDataCheck = false;
                                                                        QString message = "Pixel data, Memory access error!";
                                                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    pixelDataCheck = false;
                                                                    QStringList errorMessage;
                                                                    errorMessage.append(photoMetInter);
                                                                    errorMessage.append(" is not yet supported!");
                                                                    QString message = errorMessage.join("");
                                                                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0004)");
                                                                }
                                                            }
                                                            if(alright == true)
                                                            {
                                                                QPixmap pMap = QPixmap::fromImage(images.at(tableCounter), Qt::AutoColor);
                                                                QStandardItem * itm = new QStandardItem;
                                                                itm->setIcon(pMap);
                                                                QString n = filename.c_str();
                                                                itm->setText(n);
                                                                itm->setFlags(itm->flags() ^ Qt::ItemIsEditable);
                                                                itm->setTextAlignment(Qt::AlignBottom);
                                                                model->setItem(tableCounter, 0, itm);
                                                                correctList<<stringList.at(listCounter);
                                                                imageBoundaries.append(boxBoundaries(0, cols, 0, rows));
                                                                tableCounter++;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            pixelDataCheck = false;
                                                            QStringList errorMessage;
                                                            errorMessage.append("Sample per pixel, ");
                                                            errorMessage.append(sPP.text());
                                                            QString message = errorMessage.join("");
                                                            errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0002)");
                                                        }
                                                    }
                                                    else
                                                    {
                                                        pixelDataCheck = false;
                                                        QStringList errorMessage;
                                                        errorMessage.append("Uncompressed Frame Size, ");
                                                        errorMessage.append(UCSizeE.text());
                                                        QString message = errorMessage.join("");
                                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "No tag");
                                                    }
                                                }
                                                else
                                                {
                                                    pixelDataCheck = false;
                                                    QStringList errorMessage;
                                                    errorMessage.append("Dicom Element, ");
                                                    errorMessage.append(elemError.text());
                                                    QString message = errorMessage.join("");
                                                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0,"(7FE0, 0010)");
                                                }
                                            }
                                            else
                                            {
                                                pixelDataCheck = false;
                                                QStringList errorMessage;
                                                errorMessage.append("Photometric interpretation, ");
                                                errorMessage.append(photometInt.text());
                                                QString message = errorMessage.join("");
                                                errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0004)");
                                            }
                                        }
                                        else
                                        {
                                            pixelDataCheck = false;
                                            modalInd = modality.c_str();
                                            errorFileLog(filename.c_str(), "CT or MR modality not supported yet!", 0, "(0008, 0060)");
                                        }
                                    }
                                    else
                                    {
                                        pixelDataCheck = false;
                                        errorFileLog(filename.c_str(), getModalErr.text(), 0, "(0008, 0060)");
                                    }
                                    //put it here
                                }
                                //                                    }
                                //                                    else
                                //                                    {
                                //                                        pixelDataCheck = false;
                                //                                        QString message = "Error in decompression!";
                                //                                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0,"(7FE0, 0010)");
                                //                                    }
                                DJDecoderRegistration::cleanup();
                            }
                            else
                            {
                                pixelDataCheck = false;
                                QStringList errorMessage;
                                errorMessage.append("Getting column, ");
                                errorMessage.append(colCheck.text());
                                QString message = errorMessage.join("");
                                errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0011)");
                            }
                        }
                        else
                        {   pixelDataCheck = false;
                            QStringList errorMessage;
                            errorMessage.append("Getting row, ");
                            errorMessage.append(rowCheck.text());
                            QString message = errorMessage.join("");
                            errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(0028, 0010)");
                        }
                    }
                    else
                    {   pixelDataCheck = false;
                        QString message = "Pixel data not present or not accessible!";
                        errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "(7FE0, 0010)");
                    }
                    if(pixelDataCheck == false)
                    {
                        QPixmap pMap;
                        if(strcmpi(modalInd.c_str(), "CT") == 0)
                        {
                            pMap = QPixmap(":/Myfiles/CT.png");
                        }
                        else if(strcmpi(modalInd.c_str(), "MR") == 0)
                        {
                            pMap = QPixmap(":/Myfiles/MRI.png");
                        }
                        else
                        {
                            pMap = QPixmap(":/Myfiles/DCM.png");
                        }
                        QStandardItem * itm = new QStandardItem;
                        itm->setIcon(pMap);
                        QString n = stringList.at(listCounter);
                        itm->setText(n);
                        itm->setFlags(itm->flags() ^ Qt::ItemIsEditable);
                        itm->setTextAlignment(Qt::AlignBottom);
                        defectedModel->setItem(errorCounter, 0, itm);
                        defectedPixelData<<stringList.at(listCounter);
                        errorCounter++;
                    }
                    dataset->clear();
                }
                else
                {
                    QStringList errorMessage;
                    errorMessage.append("Loading file, ");
                    errorMessage.append(loadFileError.text());
                    QString message = errorMessage.join("");
                    errorFileLog(filename.c_str(), message.toLocal8Bit().constData(), 0, "No tag");
                }
                fileformat.clear();
            }
            progress.setValue(listsize);
            if(correctList.size()>0)
            {
                ui->listOfImages->setModel(model);
                ui->listOfImages->setStyleSheet(QString("icon-size: 150px 150px"));
                ui->listOfImages->verticalHeader()->setVisible(false);
                ui->listOfImages->horizontalHeader()->setVisible(false);
                ui->listOfImages->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
                for(int i=0; i<=tableCounter; i++)
                {
                    ui->listOfImages->setRowHeight(i,100);
                    ui->listOfImages->setColumnWidth(i,300);
                }
            }
            if(defectedPixelData.size()>0)
            {
                ui->listOfdefectedPixelData->setModel(defectedModel);
                ui->listOfdefectedPixelData->setStyleSheet(QString("icon-size: 150px 150px"));
                ui->listOfdefectedPixelData->verticalHeader()->setVisible(false);
                ui->listOfdefectedPixelData->horizontalHeader()->setVisible(false);
                ui->listOfdefectedPixelData->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
                for(int i=0; i<=errorCounter; i++)
                {
                    ui->listOfdefectedPixelData->setRowHeight(i,100);
                    ui->listOfdefectedPixelData->setColumnWidth(i,300);
                }
            }
            if(wrongList.size()>0)
            {
                ui->WrongList->clear();
                ui->WrongList->addItems(wrongList);
                ui->WrongList->show();
            }
            if(correctList.size()>0)
            {
                regionValues(0);
            }
        }
    }
    delete fileDialog;
}

void MainWindow::tableClicked(const QModelIndex& index)
{
    counter = 0;
    ui->progressBar->setValue(counter);
    ui->dontShowError->setChecked(false);
    if(fetch == true)
    {
        bool check = false;
        int Serial = index.row();
        if(selectedImages.size()>0)
        {
            for(int i=0; i<selectedImages.size(); i++)
            {
                if(selectedImages.at(i)==Serial)
                {
                    selectedList.removeAt(i);
                    ui->selectedFiles->clear();
                    ui->selectedFiles->addItems(selectedList);
                    ui->selectedFiles->show();
                    selectedImages.remove(i);
                    duplicateList.remove(i);
                    check = true;
                }
            }
        }
        if(check==false)
        {
            if((Serial)<correctList.size())
            {
                selectedImages.append(Serial);
                duplicateList.append(Serial);
                selectedList<<correctList.at(selectedImages.at(selectedImages.size()-1));
                ui->selectedFiles->clear();
                ui->selectedFiles->addItems(selectedList);
                ui->selectedFiles->show();
                if(selectedImages.size() == 1)
                {
                    regionValues(selectedImages.at(0));
                }
            }
        }
    }
    else
    {
        serial = index.row();
        if(serial-1<correctList.size())
        {
            if(selectedImages.size()==0)
            {
                selectedImages.append(serial);
                duplicateList.append(serial);
                selectedList<<correctList.at(selectedImages.at(0));
                ui->selectedFiles->clear();
                ui->selectedFiles->addItems(selectedList);
                ui->selectedFiles->show();
            }
            regionValues(selectedImages.at(0));
        }
    }
}

void MainWindow::showBigImage()
{
    if(selectedImages.size() > 0)
    {
        ui->listOfImages->setAttribute(Qt::WA_Disabled, true);
        ui->bigImageLabel->clear();
        QSize bigsize = ui->bigImageLabel->size();
        ui->bigImageLabel->setPixmap(image->scaled(bigsize, Qt::IgnoreAspectRatio, Qt::FastTransformation));
        ui->bigImageLabel->show();
        ui->bigImageLabel->activateWindow();
        ui->closeBigImage->show();
        ui->modifyTheImage->show();
        ui->progressBar->show();
        ui->refresh->show();
        ui->dontShowError->setChecked(false);
        ui->dontShowError->show();
        ui->boxes->show();
        ui->boxesLabel->show();
        ui->prevFileCheck->hide();
        ui->label->hide();
        ui->selectFile->hide();
        ui->destSelect->hide();
        ui->ErrorButton->hide();
        ui->errorEdit->hide();
        ui->errorClose->hide();
        ui->clearError->hide();
        ui->clearButton->hide();
        ui->listLabel->hide();
        ui->listOfImages->hide();
        ui->listDPD->hide();
        ui->listOfdefectedPixelData->hide();
        ui->selectAll->hide();
        ui->tagChanger->hide();
        ui->nonUIoption->hide();
        bigImageShown = true;
    }
}

void MainWindow::closeBigImage()
{
    if(inModifyFunc == true)
    {
        emergeBreak = true;
    }
    else
    {
        refresher();
        ui->boxes->hide();
        ui->boxesLabel->hide();
        ui->bigImageLabel->clear();
        ui->bigImageLabel->hide();
        ui->closeBigImage->hide();
        ui->modifyTheImage->hide();
        ui->progressBar->hide();
        ui->dontShowError->hide();
        ui->prevFileCheck->show();
        ui->selectFile->show();
        ui->destSelect->show();
        ui->label->show();
        ui->ErrorButton->show();
        ui->clearButton->show();
        ui->listLabel->show();
        ui->selectAll->show();
        ui->tagChanger->show();
        ui->listOfImages->setAttribute(Qt::WA_Disabled, false);
        ui->listOfImages->show();
        ui->listDPD->show();
        ui->nonUIoption->show();
        ui->listOfdefectedPixelData->show();
        ui->refresh->hide();
        bigImageShown = false;
        inModifyFunc = false;
        if(errorLogOpen == true)
        {
            showError();
        }

        if(overrideCursor == true)
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
            enableInputs();
            overrideCursor = false;
        }
    }
}

void MainWindow::selectTheRegion(int ind, QVector<boxBoundaries>&boundaryList)
{
    for(int i = 0; i<ui->bigImageLabel->bandList.size(); i++)
    {
        QRect rectMargin = ui->bigImageLabel->bandList.at(i)->geometry();
        QSize bigsize = ui->bigImageLabel->size();
        double hratio = (double)bigsize.height()/(double)imageBoundaries.at(ind).ymax_r;
        double wratio = (double)bigsize.width()/(double)imageBoundaries.at(ind).xmax_r;
        int xmin_temp = rectMargin.left();
        int xmax_temp = rectMargin.right();
        int ymin_temp = rectMargin.top();
        int ymax_temp = rectMargin.bottom();
        xmin_temp = (long int)((int)((double)xmin_temp/wratio));
        xmax_temp = (long int)((int)((double)xmax_temp/wratio));
        ymin_temp = (long int)((int)((double)ymin_temp/hratio));
        ymax_temp = (long int)((int)((double)ymax_temp/hratio));
        xmin_temp=xmin_temp<0?0:xmin_temp;
        xmax_temp=xmax_temp<0?0:xmax_temp;
        ymin_temp=ymin_temp<0?0:ymin_temp;
        ymax_temp=ymax_temp<0?0:ymax_temp;
        boxBoundaries tempBox(xmin_temp, xmax_temp, ymin_temp, ymax_temp);
        boundaryList.append(tempBox);
    }
}

void MainWindow::enableInputs()
{
    ui->modifyTheImage->setEnabled(true);
    ui->refresh->setEnabled(true);
    ui->selectedFiles->setEnabled(true);
    ui->WrongList->setEnabled(true);
    ui->finishedList->setEnabled(true);
    ui->bigImageLabel->setEnabled(true);
    ui->dontShowError->setEnabled(true);
    ui->progressBar->setEnabled(true);
    ui->dontShowError->setEnabled(true);
}

void MainWindow::modifyingTheImage()
{
    if(destinationSelected == false)
    {
        QStringList pathBuild;
        QDir dir;
        QStringList splitStr = correctList.at(selectedImages.at(0)).split("/");
        for(int wordCounter = 0; wordCounter<splitStr.size()-1; wordCounter++)
        {
            pathBuild.append(splitStr.at(wordCounter));
            pathBuild.append("/");
        }
        dir.setPath(pathBuild.join(""));
        pathBuild.append("Modified Files");
        QString destPath = pathBuild.join("");
        if(dir.exists(destPath)==false)
        {
            bool fileConstruct = dir.mkdir(destPath);
            if(fileConstruct == true)
            {
                destDirectory.setPath(destPath);
                destinationSelected = true;
            }
            else
            {
                QMessageBox::information(this, "Message!", "No permission to write in the desired directory!");
            }
        }
        else
        {
            destDirectory.setPath(destPath);
            destinationSelected = true;
        }
    }
    if(destinationSelected == true)
    {
        if(destDirectory.exists()!=true)
        {
            destDirectory.mkdir(destDirectory.absolutePath());
        }
        inModifyFunc = true;
        emergeBreak = false;
        DJDecoderRegistration::registerCodecs(/*EDC_photometricInterpretation, EUC_default, EPC_default, OFFalse*/);// register JPEG codecs
        if(selectedImages.size() > 0)
        {
            ui->progressBar->setMaximum(ui->selectedFiles->count());
            QApplication::processEvents(QEventLoop::AllEvents);
            while(selectedImages.size()>0)
            {
                if(emergeBreak == true)
                {
                    inModifyFunc = false;
                    emergeBreak = false;
                    closeBigImage();
                    break;
                }
                counter++;
                ui->selectedFiles->item(ui->selectedFiles->count()-selectedImages.size())->setSelected(true);
                ui->selectedFiles->update();
                ui->selectedFiles->show();
                QApplication::processEvents(QEventLoop::AllEvents);
                QApplication::setOverrideCursor(Qt::WaitCursor);
                ui->modifyTheImage->setDisabled(true);
                ui->refresh->setDisabled(true);
                ui->selectedFiles->setDisabled(true);
                ui->WrongList->setDisabled(true);
                ui->finishedList->setDisabled(true);
                ui->bigImageLabel->setDisabled(true);
                ui->dontShowError->setDisabled(true);
                ui->progressBar->setDisabled(true);
                ui->dontShowError->setDisabled(true);
                overrideCursor = true;
                string destinationFilename;
                string hash="/";
                destinationFilename = destDirectory.path().toLocal8Bit().constData()+hash+correctList.at(selectedImages.at(0)).split("/").last().toLocal8Bit().constData();
                DcmFileFormat fileformat;
                OFCondition sourceLoadFile = fileformat.loadFile(correctList.at(selectedImages.at(0)).toLocal8Bit().constData());
                if(sourceLoadFile == EC_Normal)
                {
                    DcmDataset * dataset = fileformat.getDataset();
                    OFBool pWriteYBR422 = OFFalse;
                    const char* photoMetInter;
                    OFCondition photometInt = dataset->findAndGetString(DCM_PhotometricInterpretation, photoMetInter, true);
                    if(photometInt == EC_Normal)
                    {
                        if(strcmpi(photoMetInter, "YBR_FULL_422")==0)
                        {
                            pWriteYBR422 = OFTrue;
                        }
                        if(tagModified == true)
                        {
                            inModifyFunc = false;
                            for(int dicomTagCounter = 0; dicomTagCounter<tagNames.size(); dicomTagCounter++)
                            {
                                QString groupTemp = "0x";
                                groupTemp.append(tagNames.at(dicomTagCounter).groupNumber);
                                QString elementTemp = "0x";
                                elementTemp.append(tagNames.at(dicomTagCounter).elementNumber);
                                bool boolGroup;
                                ushort shortGroup = groupTemp.toUShort(&boolGroup, 16);
                                bool boolElement;
                                ushort shortElement = elementTemp.toUShort(&boolElement, 16);
                                if(boolGroup == true && boolElement == true)
                                {
                                    DcmTag dTag(shortGroup, shortElement);
                                    if(dataset->tagExists(dTag.getXTag()) == true)
                                    {
                                        OFCondition tagErr = dataset->putAndInsertString(dTag.getXTag(), tagNames.at(dicomTagCounter).value.toLocal8Bit().constData(), true);
                                        if(tagErr != EC_Normal)
                                        {
                                            QString errorMessage(QString(dTag.getTagName()));
                                            errorMessage.append(", ");
                                            errorMessage.append(tagErr.text());
                                            QString tagString(QString("("));
                                            tagString.append(tagNames.at(dicomTagCounter).groupNumber);
                                            tagString.append(",");
                                            tagString.append(tagNames.at(dicomTagCounter).elementNumber);
                                            tagString.append(")");
                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), errorMessage.toLocal8Bit().constData(), selectedImages.at(0), tagString.toLocal8Bit().constData());
                                        }
                                    }
                                }
                            }
                            if(UIDChanges == true)
                            {
                                if(seriesInstanceListCF.size()==correctList.size())
                                {
                                    if(QString::compare(seriesInstanceListCF.at(selectedImages.at(0)).fileNames, correctList.at(selectedImages.at(0)), Qt::CaseInsensitive) == 0 &&
                                            QString::compare(seriesInstanceListCF.at(selectedImages.at(0)).UID, QString("Error"), Qt::CaseInsensitive)!=0)
                                    {
                                        OFCondition seriesUIDErr = dataset->putAndInsertString(DCM_SeriesInstanceUID, seriesInstanceListCF.at(selectedImages.at(0)).UID.toLocal8Bit().constData(), true);
                                        if(seriesUIDErr != EC_Normal)
                                        {
                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), seriesUIDErr.text(), 0, "(0020,000E)");
                                        }
                                    }
                                    else
                                    {
                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Series UID", 0, "(0020,000E)");
                                    }
                                }
                                else
                                {
                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Series UID", 0, "(0020,000E)");
                                }
                                if(studyInstanceListCF.size()==correctList.size())
                                {
                                    if(QString::compare(studyInstanceListCF.at(selectedImages.at(0)).fileNames, correctList.at(selectedImages.at(0)), Qt::CaseInsensitive) == 0 &&
                                            QString::compare(studyInstanceListCF.at(selectedImages.at(0)).UID, QString("Error"), Qt::CaseInsensitive)!=0)
                                    {
                                        OFCondition studyUIDErr = dataset->putAndInsertString(DCM_StudyInstanceUID, studyInstanceListCF.at(selectedImages.at(0)).UID.toLocal8Bit().constData(), true);
                                        if(studyUIDErr != EC_Normal)
                                        {
                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), studyUIDErr.text(), 0, "(0020,000D)");
                                        }
                                    }
                                    else
                                    {
                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Study UID", 0, "(0020,000D)");
                                    }

                                }
                                else
                                {
                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Error in generating new Study UID", 0, "(0020,000E)");
                                }
                            }
                        }
                        if(inModifyFunc == false)
                        {
                            inModifyFunc = true;
                        }
                        rows = 0;
                        cols = 0;
                        OFCondition Rget=dataset->findAndGetUint16(DCM_Rows, rows);
                        if(EC_Normal==Rget)
                        {
                            OFCondition Cget=dataset->findAndGetUint16(DCM_Columns, cols);
                            if(EC_Normal==Cget)
                            {
                                QVector<boxBoundaries>revisedBoundaryList;
                                revisedBoundaryList.clear();
                                selectTheRegion(selectedImages.at(0), revisedBoundaryList);
                                E_TransferSyntax xfer= dataset->getOriginalXfer();
                                OFCondition chRep;
                                bool changeState = false;
                                if(xfer != 2)
                                {
                                    chRep = dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
                                    changeState = dataset->canWriteXfer(EXS_LittleEndianExplicit);
                                }
                                else
                                {
                                    chRep = EC_Normal;
                                    changeState = true;
                                }

                                if(EC_Normal == chRep)
                                {
                                    if(changeState == true)
                                    {
                                        DcmElement* element=NULL;
                                        OFCondition DPixData = dataset->findAndGetElement(DCM_PixelData,element);
                                        if(EC_Normal==DPixData)
                                        {
                                            samplePerPixel = 0;
                                            bool planarConfigurationExists = false;
                                            Uint16 planarConfiguration = 0;
                                            Uint32 startFragment=0;
                                            sizeF=0;
                                            OFCondition UcFrameS=element->getUncompressedFrameSize(dataset,sizeF);
                                            if(EC_Normal==UcFrameS)
                                            {
                                                OFString decompressedColorModel=NULL;
                                                OFCondition DecColorM=element->getDecompressedColorModel(dataset,decompressedColorModel);
                                                if(EC_Normal==DecColorM)
                                                {
                                                    numOfFrames=0;
                                                    OFCondition samplePP=dataset->findAndGetUint16(DCM_SamplesPerPixel,samplePerPixel);
                                                    if(EC_Normal==samplePP)
                                                    {
                                                        bool goOn = true;
                                                        QString tempErrMsg;
                                                        if(dataset->tagExistsWithValue(DCM_NumberOfFrames))
                                                        {
                                                            OFCondition numOfFrame=dataset->findAndGetLongInt(DCM_NumberOfFrames,numOfFrames);
                                                            if(numOfFrame==EC_Normal)
                                                            {
                                                                if(numOfFrames<1)
                                                                {
                                                                    numOfFrames=1;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                tempErrMsg.append(QString(numOfFrame.text()));
                                                                goOn = false;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            numOfFrames=1;
                                                        }
                                                        if(goOn == true)
                                                        {
                                                            try
                                                            {
                                                                bool alright = false;
                                                                Uint8 * fullBuffer = new Uint8[(int(sizeF*numOfFrames))];
                                                                Uint16 bitsAllocated;
                                                                OFCondition bitsAlloErr = dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
                                                                if(bitsAlloErr == EC_Normal)
                                                                {
                                                                    if(samplePerPixel == 3)
                                                                    {
                                                                        if((int)bitsAllocated == 8)
                                                                        {
                                                                            if(dataset->tagExistsWithValue(DCM_PlanarConfiguration))
                                                                            {
                                                                                planarConfigurationExists=true;
                                                                                OFCondition PConf=dataset->findAndGetUint16(DCM_PlanarConfiguration,planarConfiguration);
                                                                                if(EC_Normal==PConf)
                                                                                {
                                                                                    if(int(planarConfiguration) == 0)
                                                                                    {
                                                                                        for(int i=0;i<numOfFrames;i++)
                                                                                        {
                                                                                            Uint8 * tBuffer = new Uint8[int(sizeF)];//Buffer for each frame
                                                                                            DcmFileCache * cache=NULL;
                                                                                            OFCondition UFrame=element->getUncompressedFrame(dataset,i,startFragment,tBuffer,sizeF,decompressedColorModel,cache);
                                                                                            if(EC_Normal==UFrame)
                                                                                            {
                                                                                                if(tBuffer != NULL)
                                                                                                {
                                                                                                    for(int revisedBoxCounter = 0; revisedBoxCounter<revisedBoundaryList.size(); revisedBoxCounter++)
                                                                                                    {
                                                                                                        if(revisedBoundaryList.at(revisedBoxCounter).xmin_r<cols && revisedBoundaryList.at(revisedBoxCounter).ymin_r<rows)
                                                                                                        {
                                                                                                            pl0pixelReplace(tBuffer, revisedBoundaryList.at(revisedBoxCounter).xmax_r, revisedBoundaryList.at(revisedBoxCounter).ymax_r, revisedBoundaryList.at(revisedBoxCounter).xmin_r, revisedBoundaryList.at(revisedBoxCounter).ymin_r);
                                                                                                        }
                                                                                                    }
                                                                                                    memcpy(fullBuffer+(i*sizeF), tBuffer,sizeF);//concatenate the modified frame by frame pixel data
                                                                                                }
                                                                                                alright = true;
                                                                                                delete [] tBuffer;
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                alright = false;
                                                                                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), UFrame.text(), selectedImages.at(0), "In getting uncompressed frame");
                                                                                                fileformat.clear();
                                                                                                dataset->clear();
                                                                                                delete [] fullBuffer;
                                                                                                delete [] tBuffer;
                                                                                                if(ui->dontShowError->isChecked() == false)
                                                                                                {
                                                                                                    goto stop;
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                    else if(int(planarConfiguration) == 1)
                                                                                    {
                                                                                        const Uint8 * tempBuf = 0;
                                                                                        unsigned long * count = NULL;
                                                                                        OFCondition fullBuf = dataset->findAndGetUint8Array(DCM_PixelData, tempBuf, count, OFFalse);
                                                                                        if(fullBuf == EC_Normal)
                                                                                        {
                                                                                            memcpy(fullBuffer, tempBuf,sizeF*numOfFrames);
                                                                                            for( int i=0; i<numOfFrames;i++)
                                                                                            {
                                                                                                for(int revisedBoxCounter = 0; revisedBoxCounter<revisedBoundaryList.size(); revisedBoxCounter++)
                                                                                                {
                                                                                                    if(revisedBoundaryList.at(revisedBoxCounter).xmin_r<cols && revisedBoundaryList.at(revisedBoxCounter).ymin_r<rows)
                                                                                                    {
                                                                                                        pl1pixelReplace(fullBuffer, revisedBoundaryList.at(revisedBoxCounter).xmax_r, revisedBoundaryList.at(revisedBoxCounter).ymax_r, revisedBoundaryList.at(revisedBoxCounter).xmin_r, revisedBoundaryList.at(revisedBoxCounter).ymin_r, i);
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                            alright = true;
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            fileformat.clear();
                                                                                            dataset->clear();
                                                                                            delete [] fullBuffer;
                                                                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), fullBuf.text(), selectedImages.at(0), "(7FE0, 0010)");
                                                                                            if(ui->dontShowError->isChecked() == false)
                                                                                            {
                                                                                                break;
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        fileformat.clear();
                                                                                        dataset->clear();
                                                                                        delete [] fullBuffer;
                                                                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Unknown planar configuration!", selectedImages.at(0), "In planar configuration");
                                                                                        if(ui->dontShowError->isChecked() == false)
                                                                                        {
                                                                                            break;
                                                                                        }
                                                                                    }

                                                                                }
                                                                                else
                                                                                {
                                                                                    delete [] fullBuffer;
                                                                                    fileformat.clear();
                                                                                    dataset->clear();
                                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), PConf.text(), selectedImages.at(0), "(0028, 0006)");
                                                                                    if(ui->dontShowError->isChecked() == false)
                                                                                    {
                                                                                        break;
                                                                                    }
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                delete [] fullBuffer;
                                                                                fileformat.clear();
                                                                                dataset->clear();
                                                                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Planar Configuration tag doesn't exist!", selectedImages.at(0), "(0x0028, 0x0006)");
                                                                                if(ui->dontShowError->isChecked() == false)
                                                                                {
                                                                                    break;
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            delete [] fullBuffer;
                                                                            fileformat.clear();
                                                                            dataset->clear();
                                                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "RGB image with only 8 bit allocation is supported.", selectedImages.at(0), "(0028, 0004)");
                                                                            if(ui->dontShowError->isChecked() == false)
                                                                            {
                                                                                break;
                                                                            }
                                                                        }
                                                                    }
                                                                    else if(samplePerPixel == 1)
                                                                    {
                                                                        Uint16 bitStored;
                                                                        OFCondition bitsStorErr = dataset->findAndGetUint16(DCM_BitsStored, bitStored);
                                                                        if(bitsStorErr == EC_Normal)
                                                                        {
                                                                            double factor = (int)sizeF/((int)rows * (int) cols * (int) samplePerPixel);
                                                                            double result = (cols*factor);
                                                                            for(int i=0;i<numOfFrames;i++)
                                                                            {
                                                                                Uint8 * tBuffer = new Uint8[int(sizeF)];//Buffer for each frame
                                                                                DcmFileCache * cache=NULL;
                                                                                OFCondition UFrame=element->getUncompressedFrame(dataset,i,startFragment,tBuffer,sizeF,decompressedColorModel,cache);
                                                                                if(EC_Normal==UFrame)
                                                                                {
                                                                                    if(tBuffer != NULL)
                                                                                    {
                                                                                        for(int revisedBoxCounter = 0; revisedBoxCounter<revisedBoundaryList.size(); revisedBoxCounter++)
                                                                                        {
                                                                                            int mcxmin = factor*revisedBoundaryList.at(revisedBoxCounter).xmin_r;
                                                                                            int mcxmax = factor*revisedBoundaryList.at(revisedBoxCounter).xmax_r;
                                                                                            if(mcxmin<(int)result && factor*revisedBoundaryList.at(revisedBoxCounter).ymin_r<rows)
                                                                                            {
                                                                                                nonPlanarPixelReplace(tBuffer, mcxmax, revisedBoundaryList.at(revisedBoxCounter).ymax_r, mcxmin, revisedBoundaryList.at(revisedBoxCounter).ymin_r, factor);
                                                                                            }
                                                                                        }
                                                                                        memcpy(fullBuffer+(i*sizeF), tBuffer,sizeF);//concatenate the modified frame by frame pixel data
                                                                                    }
                                                                                    delete [] tBuffer;
                                                                                    alright = true;
                                                                                }
                                                                                else
                                                                                {
                                                                                    alright = false;
                                                                                    fileformat.clear();
                                                                                    dataset->clear();
                                                                                    delete [] tBuffer;
                                                                                    delete [] fullBuffer;
                                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), UFrame.text(), selectedImages.at(0), "In getting uncompressed data!");
                                                                                    if(ui->dontShowError->isChecked() == false)
                                                                                    {
                                                                                        goto stop;
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            fileformat.clear();
                                                                            dataset->clear();
                                                                            delete [] fullBuffer;
                                                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), bitsStorErr.text(), selectedImages.at(0), "(0028, 0101)");
                                                                            if(ui->dontShowError->isChecked() == false)
                                                                            {
                                                                                break;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    fileformat.clear();
                                                                    dataset->clear();
                                                                    delete [] fullBuffer;
                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), bitsAlloErr.text(), selectedImages.at(0), "(0028, 0004)");
                                                                    if(ui->dontShowError->isChecked() == false)
                                                                    {
                                                                        break;
                                                                    }
                                                                }
                                                                if(alright == true)
                                                                {
                                                                    OFCondition fullcopy=dataset->putAndInsertUint8Array(DCM_PixelData,fullBuffer,sizeF*numOfFrames,true);
                                                                    if(fullcopy == EC_Normal)
                                                                    {
                                                                        // Commented out because I'm not sure whether or not deleting the LOSSY IMAGE COMPRESSION and LOSSY IMAGE COMPRESSION RATIO tags is a good idea.
                                                                        //                                                            if(dataset->tagExists(DCM_LossyImageCompression, true))
                                                                        //                                                            {
                                                                        //                                                                OFCondition remLIC=dataset->findAndDeleteElement(DCM_LossyImageCompression, true, true);
                                                                        //                                                                if(remLIC!=EC_Normal)
                                                                        //                                                                {
                                                                        //                                                                    fileformat.clear();
                                                                        //                                                                    dataset->clear();
                                                                        //                                                                    delete [] fullBuffer;
                                                                        //                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), remLIC.text(), selectedImages.at(0), "(0028, 2110)");
                                                                        //                                                                    break;
                                                                        //                                                                }
                                                                        //                                                            }
                                                                        //                                                            if(dataset->tagExists(DCM_LossyImageCompressionRatio, true))
                                                                        //                                                            {
                                                                        //                                                                OFCondition remLICRat=dataset->findAndDeleteElement(DCM_LossyImageCompressionRatio, true, true);
                                                                        //                                                                if(remLICRat!=EC_Normal)
                                                                        //                                                                {
                                                                        //                                                                    fileformat.clear();
                                                                        //                                                                    dataset->clear();
                                                                        //                                                                    delete [] fullBuffer;
                                                                        //                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), remLICRat.text(), selectedImages.at(0), "(0028, 2112)");
                                                                        //                                                                    break;
                                                                        //                                                                }
                                                                        //                                                            }
                                                                        //DcmItem *metaInfo = fileformat.getMetaInfo();
                                                                        DJEncoderRegistration::registerCodecs(ECC_lossyYCbCr, EUC_default, OFFalse, 0, 0, 0, OFTrue, ESS_444, pWriteYBR422, OFFalse, 0, 0, 0.0, 0.0, 0, 0, 0, 0, OFTrue, OFFalse, OFFalse, OFFalse, OFTrue);
                                                                        bool nestedAlright = true;
                                                                        try
                                                                        {
                                                                            OFCondition compressing;
                                                                            if(xfer == 12 || xfer == 11 || xfer == 24 || xfer == 19 || xfer == 20 || xfer == 21 || xfer == 22 || xfer == 23 || xfer == 25 || xfer == 26 || xfer == 30)
                                                                            {
                                                                                DJ_RPLossless params;
                                                                                compressing = dataset->chooseRepresentation(xfer, &params);
                                                                                if(compressing != EC_Normal)
                                                                                {
                                                                                    nestedAlright = false;
                                                                                    fileformat.clear();
                                                                                    dataset->clear();
                                                                                    delete [] fullBuffer;
                                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), compressing.text(), selectedImages.at(0), "In re-compressing");
                                                                                    if(ui->dontShowError->isChecked() == false)
                                                                                    {
                                                                                        break;
                                                                                    }
                                                                                }
                                                                            }
                                                                            // Still compression methods 28, 29, 31, 32, 33 not covered. Potential bug exists.
                                                                            else if(xfer == 4 || xfer == 5 || xfer == 6 || xfer == 7 || xfer == 8 || xfer == 9 || xfer == 10 || xfer == 13 || xfer == 14 || xfer == 15 || xfer == 16 || xfer == 17 || xfer == 18 || xfer == 27)
                                                                            {
                                                                                DJ_RPLossy params;
                                                                                compressing = dataset->chooseRepresentation(xfer, &params);
                                                                                if(compressing != EC_Normal)
                                                                                {
                                                                                    nestedAlright = false;
                                                                                    fileformat.clear();
                                                                                    dataset->clear();
                                                                                    delete [] fullBuffer;
                                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), compressing.text(), selectedImages.at(0), "In re-compressing");
                                                                                    if(ui->dontShowError->isChecked() == false)
                                                                                    {
                                                                                        break;
                                                                                    }
                                                                                }
                                                                            }
                                                                            else if(xfer == 2)
                                                                            {
                                                                                nestedAlright = true;
                                                                            }
                                                                            else
                                                                            {
                                                                                nestedAlright = false;
                                                                                fileformat.clear();
                                                                                dataset->clear();
                                                                                delete [] fullBuffer;
                                                                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Transfer sytax not supported!", selectedImages.at(0), "In re-compressing");
                                                                                if(ui->dontShowError->isChecked() == false)
                                                                                {
                                                                                    break;
                                                                                }
                                                                            }
                                                                        }
                                                                        catch(exception &e1)
                                                                        {
                                                                            nestedAlright = false;
                                                                            delete [] fullBuffer;
                                                                            const char* msg = e1.what();
                                                                            if(overrideCursor == true)
                                                                            {
                                                                                QApplication::setOverrideCursor(Qt::ArrowCursor);
                                                                                enableInputs();
                                                                                overrideCursor = false;
                                                                            }
                                                                            fileformat.clear();
                                                                            dataset->clear();
                                                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), msg, selectedImages.at(0), "In re-compressing the pixel data!");
                                                                            if(ui->dontShowError->isChecked() == false)
                                                                            {
                                                                                break;
                                                                            }
                                                                        }
                                                                        if(nestedAlright == true)
                                                                        {
                                                                            bool reChangeState = false;
                                                                            //                                                                            if(xfer == -1)
                                                                            //                                                                            {
                                                                            //                                                                                xfer = EXS_LittleEndianExplicit;

                                                                            //                                                                            }

                                                                            if(xfer == 2)
                                                                            {
                                                                                reChangeState = true;
                                                                            }
                                                                            else
                                                                            {
                                                                                reChangeState = dataset->canWriteXfer(xfer);
                                                                            }

                                                                            if(reChangeState)
                                                                            {
                                                                                OFCondition newCond=fileformat.saveFile(destinationFilename.c_str(),xfer);
                                                                                if(newCond==EC_Normal)
                                                                                {
                                                                                    fileformat.clear();
                                                                                    dataset->clear();
                                                                                    finishedList<<destinationFilename.c_str();
                                                                                    selectedImages.remove(0);
                                                                                    ui->finishedList->clear();
                                                                                    ui->finishedList->addItems(finishedList);
                                                                                    ui->finishedList->show();
                                                                                    delete [] fullBuffer;
                                                                                    if(selectedImages.size()==0)
                                                                                    {
                                                                                        if(overrideCursor == true)
                                                                                        {
                                                                                            QApplication::setOverrideCursor(Qt::ArrowCursor);
                                                                                            enableInputs();
                                                                                            overrideCursor = false;
                                                                                        }
                                                                                        QApplication::processEvents(QEventLoop::AllEvents);
                                                                                        endingMsg();
                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    fileformat.clear();
                                                                                    dataset->clear();
                                                                                    delete [] fullBuffer;
                                                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), newCond.text(), selectedImages.at(0), "In saving");
                                                                                    if(ui->dontShowError->isChecked() == false)
                                                                                    {
                                                                                        break;
                                                                                    }
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                fileformat.clear();
                                                                                dataset->clear();
                                                                                delete [] fullBuffer;
                                                                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), "Cannot write with the appropriate transfer syntax!", selectedImages.at(0), "No tag");
                                                                                if(ui->dontShowError->isChecked() == false)
                                                                                {
                                                                                    break;
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        fileformat.clear();
                                                                        dataset->clear();
                                                                        delete [] fullBuffer;
                                                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), fullcopy.text(), selectedImages.at(0), "(7FE0, 0010)");
                                                                        if(ui->dontShowError->isChecked() == false)
                                                                        {
                                                                            break;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            catch(exception &e)
                                                            {
                                                                const char* msg = e.what();
                                                                if(overrideCursor == true)
                                                                {
                                                                    QApplication::setOverrideCursor(Qt::ArrowCursor);
                                                                    enableInputs();
                                                                    overrideCursor = false;
                                                                }
                                                                fileformat.clear();
                                                                dataset->clear();
                                                                QString message(msg);
                                                                message.append(" during memory allocation!");
                                                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), message.toLocal8Bit().constData(), selectedImages.at(0), "No tag");
                                                                if(ui->dontShowError->isChecked() == false)
                                                                {
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            fileformat.clear();
                                                            dataset->clear();
                                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), tempErrMsg.toLocal8Bit().constData(), selectedImages.at(0), "(0028, 0008)");
                                                            if(ui->dontShowError->isChecked() == false)
                                                            {
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        fileformat.clear();
                                                        dataset->clear();
                                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), samplePP.text(), selectedImages.at(0), "(0x0028, 0x0002)");
                                                        if(ui->dontShowError->isChecked() == false)
                                                        {
                                                            break;
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    fileformat.clear();
                                                    dataset->clear();
                                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), DecColorM.text(), selectedImages.at(0), "No tag");
                                                    if(ui->dontShowError->isChecked() == false)
                                                    {
                                                        break;
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                fileformat.clear();
                                                dataset->clear();
                                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), UcFrameS.text(), selectedImages.at(0), "No tag");
                                                if(ui->dontShowError->isChecked() == false)
                                                {
                                                    break;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            fileformat.clear();
                                            dataset->clear();
                                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), DPixData.text(), selectedImages.at(0), "(7FE0, 0010)");
                                            if(ui->dontShowError->isChecked() == false)
                                            {
                                                break;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        const char* msg = " cannot be modified as decompression is not possible.";
                                        fileformat.clear();
                                        dataset->clear();
                                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), msg, selectedImages.at(0), "In decompressing!");
                                        if(ui->dontShowError->isChecked() == false)
                                        {
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    fileformat.clear();
                                    dataset->clear();
                                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), chRep.text(), selectedImages.at(0), "in choosing representation!");
                                    if(ui->dontShowError->isChecked() == false)
                                    {
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                fileformat.clear();
                                dataset->clear();
                                errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), Cget.text(), selectedImages.at(0), "(0028, 0011)");
                                if(ui->dontShowError->isChecked() == false)
                                {
                                    break;
                                }
                            }
                        }
                        else
                        {
                            fileformat.clear();
                            dataset->clear();
                            errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), Rget.text(), selectedImages.at(0), "(0028, 0010)");
                            if(ui->dontShowError->isChecked() == false)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        fileformat.clear();
                        dataset->clear();
                        errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), photometInt.text(), selectedImages.at(0), "(0028, 0004)");
                        if(ui->dontShowError->isChecked() == false)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    fileformat.clear();
                    errorFileLog(correctList.at(selectedImages.at(0)).toLocal8Bit().constData(), sourceLoadFile.text(), selectedImages.at(0), "No tag");
                    if(ui->dontShowError->isChecked() == false)
                    {
                        break;
                    }
                }
                ui->progressBar->setValue(counter);
            }
        }
stop:
        {
            printf_s("Stopped while changing the pixel data!");
        }
        DJEncoderRegistration::cleanup();
        DJDecoderRegistration::cleanup();
    }
    else
    {
        QMessageBox::information(this, "Message!", "Desitnation folder could not be created! Please select a destination folder!");
    }
}

mostFrequent MainWindow::frequencyFinder(std::vector<int>b)
{
    int maxfreq=0;
    int value = 0;
    int size = b.size();
    if(size>0)
    {
        int index = 0;
        int freq=0;
        for(int i=0; i<(size-1);i++)
        {
            if(b.at(i)==b.at(i+1))
            {
                freq++;
                if(freq > maxfreq)
                {
                    maxfreq=freq;
                    index=i;
                }
            }
            else
            {
                freq = 1;
                value = b.at(index);
            }
        }
    }
    if(maxfreq==(size-1))
    {
        value = b.at(0);
    }
    return mostFrequent(value, maxfreq);
}

void MainWindow::errorFileLog(const char* fileName, const char* status, int fileNumber, const char* tag)
{
    QFile myFile(fileName);
    int size = 0;
    if (myFile.open(QIODevice::ReadOnly))
    {
        size = myFile.size();
        myFile.close();
    }
    ui->progressBar->setValue(counter);
    QString errMsg = "";
    errMsg.append("File : ");
    errMsg.append(fileName);
    errMsg.append(", Problem : ");
    errMsg.append(status);
    errMsg.append(", Tag : ");
    errMsg.append(tag);
    errMsg.append(", File Size : ");
    errMsg.append(QString::number(size));
    errMsg.append(" Bytes.");
    errMsg.append("\n\n");
    ui->errorEdit->append(errMsg);
    bool already = false;
    for(int i=0; i<wrongList.size();i++)
    {
        if(strcmpi(wrongList.at(i).toLocal8Bit().constData(),fileName)==0)
        {
            already = true;
        }
    }
    if(already == false)
    {
        wrongList<<fileName;
    }
    if(wrongList.size()>0)
    {
        ui->WrongList->clear();
        ui->WrongList->addItems(wrongList);
        ui->WrongList->show();
    }
    if(inModifyFunc == true)
    {
        corruptedFiles.append(fileNumber);
        if(ui->dontShowError->isChecked() == false)
        {
            if(overrideCursor == true)
            {
                QApplication::setOverrideCursor(Qt::ArrowCursor);
                enableInputs();
                overrideCursor = false;
            }
            QMessageBox::information(this, "Message", errMsg);
            selectedImages.remove(0);
            if(selectedImages.size()==0)
            {
                endingMsg();
            }
            else
            {
                regionValues(selectedImages.at(0));
                inModifyFunc = false;
            }
        }
        else if(ui->dontShowError->isChecked() == true)
        {
            selectedImages.remove(0);
            if(selectedImages.size()==0)
            {
                if(overrideCursor == true)
                {
                    QApplication::setOverrideCursor(Qt::ArrowCursor);
                    enableInputs();
                    overrideCursor = false;
                }
                endingMsg();
            }
            //            else
            //            {
            //                modifyingTheImage();
            //            }
        }
    }
}

void MainWindow::nonPlanarPixelReplace(Uint8 * tBuffer, int maxx, int maxy, int minx, int miny, double factor)
{
    mostFrequent maxValue(0,0);
    double result = (cols*factor);
    maxx = maxx>=(int)result?((int)result-1):maxx;
    maxy = maxy>=rows?(rows-1):maxy;
    if(maxx>0 && maxy>0)
    {
        //First for minimum value of y boundary
        int indexA = ((int)minx + ((int)miny*factor*(int)cols))*(int)samplePerPixel;
        int indexB = ((int)maxx + ((int)miny*factor*(int)cols))*(int)samplePerPixel;
        std::vector<int>XlowY(tBuffer+indexA, tBuffer+indexB);
        //Now for maximum value of y boundary
        indexA = ((int)minx + ((int)maxy*factor*(int)cols))*(int)samplePerPixel;
        indexB = ((int)maxx + ((int)maxy*factor*(int)cols))*(int)samplePerPixel;
        std::vector<int>XhighY(tBuffer+indexA, tBuffer+indexB);
        for(unsigned long y = (unsigned long)miny; y < (unsigned long)maxy ; y++)
        {
            indexA=((int)minx + ((int)y*factor*(int)cols))*(int)samplePerPixel;
            XlowY.push_back((int)tBuffer[indexA]);
            indexB=((int)maxx + ((int)y*factor*(int)cols))*(int)samplePerPixel;
            XhighY.push_back((int)tBuffer[indexB]);
        }
        std::sort(XhighY.begin(), XhighY.end());
        maxValue = frequencyFinder(XhighY);
        XhighY.clear();
        std::sort(XlowY.begin(), XlowY.end());
        mostFrequent temp = frequencyFinder(XlowY);
        maxValue.value = maxValue.frequency>temp.frequency?maxValue.value:temp.value;
        XlowY.clear();
    }
    int index = 0;
    for(unsigned long y = (unsigned long)miny; y < (unsigned long)maxy ; y++)
    {
        for(unsigned long x = (unsigned long)minx; x < (unsigned long)maxx; x++)
        {
            index=((int)x + ((int)y*factor*(int)cols))*(int)samplePerPixel;
            if(index<int(sizeF))
            {
                tBuffer[index] = maxValue.value;
            }
        }
    }
}

void MainWindow::pl0pixelReplace(Uint8 * tBuffer, int maxx, int maxy, int minx, int miny)
{
    mostFrequent maxR(0,0);
    mostFrequent maxG(0,0);
    mostFrequent maxB(0,0);
    int index = 0;
    maxx = maxx>=cols?(cols-1):maxx;
    maxy = maxy>=rows?(rows-1):maxy;
    if(maxx>0 && maxy>0)
    {
        std::vector<int>XRlowY;
        std::vector<int>XGlowY;
        std::vector<int>XBlowY;
        std::vector<int>XRhighY;
        std::vector<int>XGhighY;
        std::vector<int>XBhighY;
        for(unsigned long x = (unsigned long)minx; x < (unsigned long)maxx; x++)
        {
            index=((int)x + ((int)miny*(int)cols))*(int)samplePerPixel;
            XRlowY.push_back((int)tBuffer[index]);
            XGlowY.push_back((int)tBuffer[index+1]);
            XBlowY.push_back((int)tBuffer[index+2]);
            index=((int)x + ((int)maxy*(int)cols))*(int)samplePerPixel;
            XRhighY.push_back((int)tBuffer[index]);
            XGhighY.push_back((int)tBuffer[index+1]);
            XBhighY.push_back((int)tBuffer[index+2]);
        }
        for(unsigned long y = (unsigned long)miny; y < (unsigned long)maxy ; y++)
        {
            index=((int)minx + ((int)y*(int)cols))*(int)samplePerPixel;
            XRlowY.push_back((int)tBuffer[index]);
            XGlowY.push_back((int)tBuffer[index+1]);
            XBlowY.push_back((int)tBuffer[index+2]);
            index=((int)maxx + ((int)y*(int)cols))*(int)samplePerPixel;
            XRhighY.push_back((int)tBuffer[index]);
            XGhighY.push_back((int)tBuffer[index+1]);
            XBhighY.push_back((int)tBuffer[index+2]);
        }
        std::sort(XRhighY.begin(), XRhighY.end());
        maxR = frequencyFinder(XRhighY);
        XRhighY.clear();
        std::sort(XRlowY.begin(), XRlowY.end());
        mostFrequent temp = frequencyFinder(XRlowY);
        maxR.value = maxR.frequency>temp.frequency?maxR.value:temp.value;
        XRlowY.clear();
        std::sort(XGhighY.begin(), XGhighY.end());
        maxG = frequencyFinder(XGhighY);
        XGhighY.clear();
        std::sort(XGlowY.begin(), XGlowY.end());
        temp = frequencyFinder(XGlowY);
        maxG.value = maxG.frequency>temp.frequency?maxG.value:temp.value;
        XGlowY.clear();
        std::sort(XBhighY.begin(), XBhighY.end());
        maxB = frequencyFinder(XBhighY);
        XBhighY.clear();
        std::sort(XBlowY.begin(), XBlowY.end());
        temp = frequencyFinder(XBlowY);
        maxB.value = maxB.frequency>temp.frequency?maxB.value:temp.value;
        XBlowY.clear();
    }
    for(unsigned long y = (unsigned long)miny; y < (unsigned long)maxy ; y++)
    {
        for(unsigned long x = (unsigned long)minx; x < (unsigned long)maxx; x++)
        {
            index=((int)x + ((int)y*(int)cols))*(int)samplePerPixel;
            if(index<int(sizeF)-2)
            {
                tBuffer[index] = maxR.value;
                tBuffer[index + 1] = maxG.value;
                tBuffer[index + 2] = maxB.value;
            }
        }
    }
}


void MainWindow::pl1pixelReplace(Uint8 * fullBuffer, int maxx, int maxy, int minx, int miny, int i)
{
    mostFrequent maxR(0,0);
    mostFrequent maxG(0,0);
    mostFrequent maxB(0,0);
    maxx = maxx>=cols?(cols-1):maxx;
    maxy = maxy>=rows?(rows-1):maxy;
    if(maxx>0 && maxy>0)
    {
        //First in X direction

        //For the minimum Y values
        //For R channel
        int indexA=((int)minx + ((int)miny*(int)cols))+(i*((int)sizeF/(int)samplePerPixel));
        int indexB=((int)maxx + ((int)miny*(int)cols))+(i*((int)sizeF/(int)samplePerPixel));
        std::vector<int>XRlowY(fullBuffer+indexA, fullBuffer+indexB);

        //For G channel
        indexA = indexA + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        indexB = indexB + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        std::vector<int>XGlowY(fullBuffer+indexA, fullBuffer+indexB);

        //For B channel
        indexA = indexA + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        indexB = indexB + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        std::vector<int>XBlowY(fullBuffer+indexA, fullBuffer+indexB);

        //For the maximum Y values
        //For R channel
        indexA=((int)minx + ((int)maxy*(int)cols))+(i*((int)sizeF/(int)samplePerPixel));
        indexB=((int)maxx + ((int)maxy*(int)cols))+(i*((int)sizeF/(int)samplePerPixel));
        std::vector<int>XRhighY(fullBuffer+indexA, fullBuffer+indexB);

        //For G channel
        indexA = indexA + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        indexB = indexB + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        std::vector<int>XGhighY(fullBuffer+indexA, fullBuffer+indexB);

        //For B channel
        indexA = indexA + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        indexB = indexB + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
        std::vector<int>XBhighY(fullBuffer+indexA, fullBuffer+indexB);

        //Now in Y direction
        for(unsigned long y = (unsigned long)miny; y < (unsigned long)maxy ; y++)
        {
            //R channel
            indexA=((int)minx + ((int)y*(int)cols))+(i*((int)sizeF/(int)samplePerPixel));
            XRlowY.push_back((int)fullBuffer[indexA]);
            //G channel
            indexA = indexA + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
            XGlowY.push_back((int)fullBuffer[indexA]);
            //B channel
            indexA = indexA + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
            XBlowY.push_back((int)fullBuffer[indexA]);
            //R channel
            indexB=((int)maxx + ((int)y*(int)cols))+(i*((int)sizeF/(int)samplePerPixel));
            XRhighY.push_back((int)fullBuffer[indexB]);
            //G channel
            indexB = indexB + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
            XGhighY.push_back((int)fullBuffer[indexB]);
            //B channel
            indexB = indexB + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
            XBhighY.push_back((int)fullBuffer[indexB]);
        }

        //For R Channel
        std::sort(XRhighY.begin(), XRhighY.end());
        maxR = frequencyFinder(XRhighY);
        XRhighY.clear();
        std::sort(XRlowY.begin(), XRlowY.end());
        mostFrequent temp = frequencyFinder(XRlowY);
        maxR.value = maxR.frequency>temp.frequency?maxR.value:temp.value;
        XRlowY.clear();
        //For G Channel
        std::sort(XGhighY.begin(), XGhighY.end());
        maxG = frequencyFinder(XGhighY);
        XGhighY.clear();
        std::sort(XGlowY.begin(), XGlowY.end());
        temp = frequencyFinder(XGlowY);
        maxG.value = maxG.frequency>temp.frequency?maxG.value:temp.value;
        XGlowY.clear();
        //For B channel
        std::sort(XBhighY.begin(), XBhighY.end());
        maxB = frequencyFinder(XBhighY);
        XBhighY.clear();
        std::sort(XBlowY.begin(), XBlowY.end());
        temp = frequencyFinder(XBlowY);
        maxB.value = maxB.frequency>temp.frequency?maxB.value:temp.value;
        XBlowY.clear();
    }
    int index = 0;
    //y=row
    //x=column
    //So, this is a row major approach
    for(unsigned long y = (unsigned long)miny; y < (unsigned long)maxy ; y++)
    {
        for(unsigned long x = (unsigned long)minx; x < (unsigned long)maxx; x++)
        {
            index=((int)x + ((int)y*(int)cols))+(i*((int)sizeF/(int)samplePerPixel));
            fullBuffer[index] = maxR.value;
            index = index + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
            fullBuffer[index] = maxG.value;
            index = index + ((int)numOfFrames*((int)sizeF/(int)samplePerPixel));
            fullBuffer[index] = maxB.value;
        }
    }
}


void MainWindow::imageChange(const QModelIndex& index)
{
    if(inModifyFunc == false && bigImageShown == true)
    {
        if(duplicateList.size()>0)
        {
            regionValues(duplicateList.at(index.row()));
        }
    }
}

void MainWindow::clearErrorLog()
{
    ui->errorEdit->clear();
}

void MainWindow::regionValues(int ind)
{
    if(images.size()>ind)
    {
        QImage imageData(images[ind]);
        if(deleteImage == true)
        {
            delete image;
            deleteImage = false;
        }
        image = new QPixmap();
        deleteImage = true;
        bool conv = image->convertFromImage(imageData, Qt::AutoColor);
        if(conv)
        {
            if(bigImageShown == false)
            {
                ui->label->setFixedSize( 400, 300);
                QSize size = ui->label->size();
                ui->label->setPixmap(image->scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation));
                ui->label->show();
            }
            else
            {
                showBigImage();
            }
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}
