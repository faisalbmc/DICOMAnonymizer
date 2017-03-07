#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDir>
#include <string.h>
#include <QRubberBand>
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimage/diregist.h"
#include "dcmtk/dcmjpeg/djdecode.h"
#include "dcmtk/dcmjpeg/djencode.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <QPoint>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QItemSelectionModel>
#include <QProgressDialog>
#include <vector>
#include <QListWidgetItem>
#include "tabledialog.h"
#include "pathdialog.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class mostFrequent
{
public:
    int value;
    int frequency;
    mostFrequent(int a, int b)
    {
        value = a;
        frequency = b;
    }
};

class IDList
{
public:
    QString UID;
    QString fileNames;
    IDList()
    {}
    IDList(QString a, QString b)
    {
        UID = b;
        fileNames = a;
    }
};

struct manufacturerImageStructure
{
public:
    QString manufacturerName;
    int horizontalRatio;
    int verticalRatio;
    int startX;
    int startY;
    int endX;
    int endY;
    int errorStatus;//0 = no error, 1 = error
    QString errorMessage;
};

struct changeInfo
{
public:
    ushort group;
    ushort element;
    int changeStatus;//0 if delete, 1 if change, 2 with error
    QString value;// empty if delete, something with change
    QString errorMessage;
};

class boxBoundaries
{
public:
    int xmin_r;
    int xmax_r;
    int ymin_r;
    int ymax_r;
    boxBoundaries()
    {
        xmin_r = 0;
        xmax_r = 0;
        ymin_r = 0;
        ymax_r = 0;
    }
    boxBoundaries(int a, int b, int c, int d)
    {
        xmin_r = a;
        xmax_r = b;
        ymin_r = c;
        ymax_r = d;
    }
};

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

class UIDTempHolder
{
public:
    QString originalValue;
    QString newValue;
    UIDTempHolder()
    {}
    UIDTempHolder(QString a, QString b)
    {
        originalValue = a;
        newValue = b;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QStringList stringList;
    QStringList wrongList;
    QStringList correctList;
    QStringList finishedList;
    QStringList selectedList;
    QStringList defectedPixelData;
public:
    explicit MainWindow(QWidget *parent = 0);
    QString PatID;
    QString fullName;
    QString dateOfBirth;
    QString gender;
    QString studyDate;
    QString studyDescription;
    QString accessionNumber;
    QString instituteName;
    QString stationName;
    QString instDeptName;
    QString operatorName;
    //    QString studyInstanceUID;
    //    QString seriesInstanceUID;
    //    QString studyID;
    ~MainWindow();
    //Variables

public slots:
    void showTheSelectedList();
    void showBigImage();
    void closeBigImage();
    void selectTheRegion(int, QVector<boxBoundaries>&);
    void modifyingTheImage();
    void destinationDirectory();
    void clearFiles();
    void tableClicked(const QModelIndex&);
    void selectAllImage();
    void showError();
    void closeError();
    void tagChanging();
    void refresher();
    void clearErrorLog();
    void imageChange(const QModelIndex&);
    void checkNOofBoxes();
    void mediator();
    void dataChanged(QStandardItem*);
    void nonUIoperation();

protected:
    bool eventFilter(QObject*, QEvent *);

private:
    Ui::MainWindow *ui;
//    subDialog * dialog;
    tableDialog * tagDialog;
    PathDialog * pathDialog;
    QDir directory;
    QDir destDirectory;
    string imageName;
    QVector<boxBoundaries>imageBoundaries;
    Uint16 samplePerPixel;
    Uint32 sizeF;//size of a total frame
    long int numOfFrames;
    Uint16 rows;
    Uint16 cols;
    Uint16 original_rows;
    Uint16 original_cols;
    int counter;

    QPixmap* image;

    QStandardItemModel * model;
    QStandardItemModel * defectedModel;

    QVector<QImage>images;
    QVector<int>selectedImages;
    QVector<int>duplicateList;
    QVector<int>corruptedFiles;

    //For Study Instance UID
    QVector<IDList> studyInstanceListCF;
    //For Series Insancee UID
    QVector<IDList> seriesInstanceListCF;
    //For Study Instance UID
    QVector<IDList> studyInstanceListDF;
    //For Series Insancee UID
    QVector<IDList> seriesInstanceListDF;
    //Boolean for UID changes
    bool UIDChanges;

    QVector<manufacturerImageStructure> imgStructList;
    QVector<changeInfo>changeInfoList;

    //Tag list dialog elements
    //Boolean for tag listing subtable
    bool tagModelStarted;
    //List of indexes that are changed!
    QVector<int>changeList;



    //boolean and relevant varibles used for selecting sets from table
    //    bool inShift;
    bool fetch;
    int serial;
    int controlButtonCounter;

    //boolean used for closing and opening error log
    bool errorLogOpen;


    //boolean used during image modification loop
    bool tagModified;
    bool overrideCursor;
    bool inModifyFunc;
    bool bigImageShown;
    bool emergeBreak;

    //List of Booleans
    //Booleans and variables used for visualisation
    int tableCounter;
    int errorCounter;
    bool firstTime;
    bool destinationSelected;
    bool deleteImage;

    //Additional tag modification or deletion part
    QVector<tagDetail> tagNames;
    int taskIndicator;//2 = modify, 1 = delete, 0 = do nothing

    //bool checkError;
    //Functions
    void IDListFillup();
    void regionValues(int);
    void endingMsg();
    void errorFileLog(const char*, const char*, int, const char*);
    char * nameTrimmer(const char*);
    void justTagModify();
    mostFrequent frequencyFinder(std::vector<int>);
    void pl1pixelReplace(Uint8*, int, int, int, int, int);
    void pl0pixelReplace(Uint8*, int, int, int, int);
    void nonPlanarPixelReplace(Uint8*, int, int, int, int, double);
    void enableInputs();
    void recurseAddDir(QDir, QStringList &);
    bool cpDir(const QString &, const QString &);
    bool rmDir(const QString &);
};

#endif // MAINWINDOW_H
