TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += FiboLine.h
HEADERS += FiboLineObject.h

SOURCES += FiboLine.cpp
SOURCES += FiboLineObject.cpp

TARGET = FiboLine.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/co
INSTALLS += target
