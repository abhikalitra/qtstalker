TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += EP.h

SOURCES += EP.cpp

TARGET = EP.0.27

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
