TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += HorizontalLine.h
HEADERS += HorizontalLineObject.h

SOURCES += HorizontalLine.cpp
SOURCES += HorizontalLineObject.cpp

TARGET = HorizontalLine.0.27

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/co
INSTALLS += target
