TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += SellArrow.h
HEADERS += SellArrowObject.h

SOURCES += SellArrow.cpp
SOURCES += SellArrowObject.cpp

TARGET = SellArrow.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/co
INSTALLS += target
