TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Line.h

SOURCES += Line.cpp

TARGET = Line

INCLUDEPATH += ../../../lib

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/chart
INSTALLS += target