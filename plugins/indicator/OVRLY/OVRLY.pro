TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += OVRLY.h

SOURCES += OVRLY.cpp

TARGET = OVRLY

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
