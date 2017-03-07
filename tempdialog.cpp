#include "tempdialog.h"
#include <QGridLayout>
#include <QDebug>
#include <QDialog>

tempDialog::tempDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(QString("Number box"));
    numberLabel = new QLabel("Number of tags to delete");
    numberOfTags = new QTextEdit("");
    numberOfTags->setFixedSize(50, 30);
    okPush = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");
    cancelButton->setFixedSize(50, 20);
    okPush->setFixedSize(30, 20);
}
