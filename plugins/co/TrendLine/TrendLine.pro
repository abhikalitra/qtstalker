TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += TrendLine.h
HEADERS += TrendLineObject.h

SOURCES += TrendLine.cpp
SOURCES += TrendLineObject.cpp

TARGET = TrendLine.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/co
INSTALLS += target
