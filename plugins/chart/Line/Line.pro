TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Line.h
HEADERS += LineDialog.h

SOURCES += Line.cpp
SOURCES += LineDialog.cpp

TARGET = Line.0.27

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
