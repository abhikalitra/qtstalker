TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += HorizontalLine.h
HEADERS += HorizontalLineObject.h

SOURCES += HorizontalLine.cpp
SOURCES += HorizontalLineObject.cpp

TARGET = HorizontalLine.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/co
INSTALLS += target
