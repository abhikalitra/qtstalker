TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += VT.h

SOURCES += VT.cpp

TARGET = VT.0.27

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
