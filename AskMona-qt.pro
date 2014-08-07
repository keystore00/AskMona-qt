
QT       += core gui webkit webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AskMona-qt
TEMPLATE = app
RESOURCES = rc.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    mywebpage.cpp \
    cookiejar.cpp \
    mywebview.cpp \
    topiclist.cpp \
    constants.cpp \
    topicview.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    mywebpage.h \
    cookiejar.h \
    mywebview.h \
    topiclist.h \
    constants.h \
    topicview.h \
    util.h

CONFIG += c++11 static
RC_FILE = icon.rc
