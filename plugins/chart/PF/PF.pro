TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += PF.h

SOURCES += PF.cpp

TARGET = PF.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
