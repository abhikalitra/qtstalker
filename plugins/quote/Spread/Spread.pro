TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Spread.h
HEADERS += SpreadDialog.h

SOURCES += Spread.cpp
SOURCES += SpreadDialog.cpp

TARGET = Spread.0.26

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
