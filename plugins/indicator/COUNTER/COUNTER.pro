TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += COUNTER.h

SOURCES += COUNTER.cpp

TARGET = COUNTER.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
