TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += MATH.h

SOURCES += MATH.cpp

TARGET = MATH.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
