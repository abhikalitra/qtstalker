TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += Yahoo.h
HEADERS += YahooDialog.h

SOURCES += Yahoo.cpp
SOURCES += YahooDialog.cpp

TARGET = Yahoo.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
