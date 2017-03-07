#include "pathdialog.h"

PathDialog::PathDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(QString("Path definer!"));
    sourceLabel = new QLabel("Put the source path here");
    destLabel = new QLabel("Put the destination path here");
    sourceText = new QTextEdit("");
    destText = new QTextEdit("");
    okPush = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");
    cancelButton->setFixedSize(50, 20);
    okPush->setFixedSize(30, 20);
}
