QT       += core gui
QT       += network
QT       += core network widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32: LIBS += -lws2_32

SOURCES += \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    udpchat.cpp

HEADERS += \
    logindialog.h \
    mainwindow.h \
    udpchat.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
