TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += LOWPASS.h
HEADERS += qtsFFT.h

SOURCES += LOWPASS.cpp
SOURCES += qtsFFT.cpp

TARGET = LOWPASS.0.29

INCLUDEPATH += .
INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target