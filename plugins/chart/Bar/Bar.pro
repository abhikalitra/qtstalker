TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Bars.h
HEADERS += BarDialog.h

SOURCES += Bars.cpp
SOURCES += BarDialog.cpp

TARGET = Bar.0.27

INCLUDEPATH += ../../../lib

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
