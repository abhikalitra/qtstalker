TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += DMI.h

SOURCES += DMI.cpp

TARGET = DMI.0.27

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
