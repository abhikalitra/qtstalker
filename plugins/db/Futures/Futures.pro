TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += Futures.h
HEADERS += FuturesDialog.h

SOURCES += Futures.cpp
SOURCES += FuturesDialog.cpp

TARGET = Futures.0.29

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/db
INSTALLS += target
