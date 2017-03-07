#-------------------------------------------------
#
# Project created by QtCreator 2013-05-24T15:40:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DCMIncognito
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        imageaction.cpp \
        rubberrect.cpp  \
        customubberband.cpp \
    subdialog.cpp \
    mdfdsman.cc \
    additionaldialog.cpp \
    tempdialog.cpp \
    tabledialog.cpp \
    pathdialog.cpp

HEADERS  += mainwindow.h \
            imageaction.h \
            rubberrect.h \
            customubberband.h \
    subdialog.h \
    mdfdsman.h \
    additionaldialog.h \
    tempdialog.h \
    tabledialog.h \
    pathdialog.h

FORMS    += mainwindow.ui
DEFINES += NOMINMAX

RC_FILE = myicon.rc

QMAKE_CFLAGS_RELEASE -= -MD
QMAKE_CFLAGS_RELEASE = -MT
QMAKE_CFLAGS_DEBUG -= -MDd
QMAKE_CFLAGS_DEBUG = -MTd
QMAKE_CXXFLAGS_RELEASE -= -MD
QMAKE_CXXFLAGS_RELEASE += -MT
QMAKE_CXXFLAGS_DEBUG -= -MDd
QMAKE_CXXFLAGS_DEBUG += -MTd

##QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO
QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcprtd.lib
QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:msvcrtd.lib
QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:libcd.lib
QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:msvcprt.lib
QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:msvcrt.lib
QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:libc.lib
##QMAKE_LFLAGS_RELEASE += /INCREMENTAL:NO

INCLUDEPATH += $$quote(D:/dcmtk-3.6.0/dcmtk-3.6.0/Prefix/include)
#INCLUDEPATH += $$quote(D:/dcmtk-3.6.0/Prefix Files/include/)

CONFIG( debug, debug|release ){
#LIBS += $$quote(-LD:/dcmtk-3.6.0/Lib Files/Debug/) \
LIBS += $$quote(-LD:/dcmtk-3.6.0/dcmtk-3.6.0/LibFiles/Debug) \
-ladvapi32\
-ldcmjpeg\
-lijg8\
-lijg12\
-lijg16\
-ldcmimage\
-ldcmimgle\
-ldcmdata\
-loflog\
-lofstd\
-lws2_32\
-lnetapi32\
-lwsock32\
-lpsapi\
}
CONFIG( release, debug|release ){
LIBS += $$quote(-LD:/dcmtk-3.6.0/dcmtk-3.6.0/LibFiles/Release/) \
#LIBS += $$quote(-LD:/Prefix/Release/) \
-ladvapi32\
-ldcmjpeg\
-lijg8\
-lijg12\
-lijg16\
-ldcmimage\
-ldcmimgle\
-ldcmdata\
-loflog\
-lofstd\
-lws2_32\
-lnetapi32\
-lwsock32\
-lpsapi\
}

RESOURCES += \
    DcmResource.qrc
