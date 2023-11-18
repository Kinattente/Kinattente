QT       += core gui
QT       += widgets
QT       += sql
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    configassistant.cpp \
    customwindow.cpp \
    kineform.cpp \
    label.cpp \
    labelpushbutton.cpp \
    main.cpp \
    mainwindow.cpp \
    options.cpp \
    overlapform.cpp \
    patientform.cpp \
    server.cpp \
    updater.cpp \
    usualfonctions.cpp \
    waitinglist.cpp \
    windowbutton.cpp \
    windowtitlebar.cpp \
    wltitle.cpp

HEADERS += \
    client.h \
    configassistant.h \
    customwindow.h \
    kineform.h \
    label.h \
    labelpushbutton.h \
    mainwindow.h \
    options.h \
    overlapform.h \
    patientform.h \
    server.h \
    updater.h \
    usualfonctions.h \
    waitinglist.h \
    windowbutton.h \
    windowtitlebar.h \
    wltitle.h

TRANSLATIONS += \
    Kinattente_fr_FR.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:RC_ICONS += \
    output.ico \

RESOURCES += \
    mystylesheet.qrc

DISTFILES += \
    Kinattente.exe.manifest
