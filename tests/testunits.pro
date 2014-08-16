QT       += widgets testlib

TARGET = testunits
INCLUDEPATH += ../
SOURCES += testunits.cpp \
    ../units.cpp

HEADERS  += ../units.h

CONFIG += c++11 testcase
