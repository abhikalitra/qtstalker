TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Yahoo.h
HEADERS += YahooDialog.h

SOURCES += Yahoo.cpp
SOURCES += YahooDialog.cpp

TARGET = Yahoo

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
