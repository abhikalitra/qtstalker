TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += FiboLine.h
HEADERS += FiboLineObject.h

SOURCES += FiboLine.cpp
SOURCES += FiboLineObject.cpp

TARGET = FiboLine.0.29

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/co
INSTALLS += target
