#-------------------------------------------------
#
# Project created by QtCreator 2014-07-03T14:29:12
#
#-------------------------------------------------

QT       += core gui multimedia #testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Player
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
     about.cpp \
    unit_test.cpp

HEADERS  += widget.h \
     about.h \
    unit_test.h

FORMS    += widget.ui \
      about.ui

RESOURCES += \
      res.qrc

RC_FILE += myicon.rc

