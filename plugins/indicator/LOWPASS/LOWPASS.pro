TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += LOWPASS.h
HEADERS += qtsFFT.h

SOURCES += LOWPASS.cpp
SOURCES += qtsFFT.cpp

TARGET = LOWPASS.0.30

INCLUDEPATH += .
INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
