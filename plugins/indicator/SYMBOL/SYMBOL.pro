TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += SYMBOL.h

SOURCES += SYMBOL.cpp

TARGET = SYMBOL.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
