#-------------------------------------------------
#
# Project created by QtCreator 2019-04-20T18:12:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArkTools
TEMPLATE = app


SOURCES += main.cpp\
        arktools.cpp \
    readmemdialog.cpp \
    driverload.cpp \
    process.cpp \
    ssdt.cpp \
    shadowssdt.cpp \
    gdt.cpp \
    idt.cpp \
    threaddialog.cpp \
    keyboard.cpp

HEADERS  += arktools.h \
    ../common/common.h \
    readmemdialog.h \
    driverload.h \
    ssdt.h \
    shadowssdt.h \
    gdt.h \
    idt.h \
    threaddialog.h \
    dispathmajorfunc.h

FORMS    += arktools.ui \
    readmemdialog.ui \
    threaddialog.ui

QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

RC_FILE = icon.rc
