TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += COMP.h

SOURCES += COMP.cpp

TARGET = COMP.0.27

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target