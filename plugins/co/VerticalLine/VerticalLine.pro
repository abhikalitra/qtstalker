TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += VerticalLine.h
HEADERS += VerticalLineObject.h

SOURCES += VerticalLine.cpp
SOURCES += VerticalLineObject.cpp

TARGET = VerticalLine.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/co
INSTALLS += target
