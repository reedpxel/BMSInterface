QT       += core gui widgets serialport testlib printsupport
CONFIG += c++17

INCLUDEPATH += $$PWD/lib
INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/forms
INCLUDEPATH += $$PWD/resources

SOURCES += \
    src/changepasswordwindow.cpp \
    src/addinfoparser.cpp \
    src/addinfowidget.cpp \
    src/batteryslider.cpp \
    src/comportreader.cpp \
    src/jbdparser.cpp \
    src/logswidget.cpp \
    src/main.cpp \
    src/maininfo.cpp \
    src/maininfoparser.cpp \
    src/maininfowidget.cpp \
    src/mainwindow.cpp \
    lib/qcustomplot.cpp \
    src/thermstatewindow.cpp

HEADERS += \
    include/changepasswordwindow.h \
    include/addinfoparser.h \
    include/addinfowidget.h \
    include/batteryslider.h \
    include/comportreader.h \
    include/jbdparser.h \
    include/logswidget.h \
    include/maininfo.h \
    include/maininfoparser.h \
    include/maininfowidget.h \
    include/mainwindow.h \
    lib/qcustomplot.h \
    include/thermstatewindow.h

FORMS += \
    forms/addinfowidget.ui \
    forms/logswidget.ui \
    forms/maininfowidget.ui \
    forms/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc
