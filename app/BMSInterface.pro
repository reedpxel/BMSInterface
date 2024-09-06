QT       += core gui widgets serialport testlib

# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addinfoparser.cpp \
    addinfowidget.cpp \
    batteryslider.cpp \
    comportreader.cpp \
    graphplotter.cpp \
    jbdparser.cpp \
    logswidget.cpp \
    main.cpp \
    maininfoparser.cpp \
    maininfowidget.cpp \
    mainwindow.cpp

HEADERS += \
    addinfoparser.h \
    addinfowidget.h \
    batteryslider.h \
    comportreader.h \
    graphplotter.h \
    jbdparser.h \
    logswidget.h \
    maininfoparser.h \
    maininfowidget.h \
    mainwindow.h

FORMS += \
    addinfowidget.ui \
    logswidget.ui \
    maininfowidget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
