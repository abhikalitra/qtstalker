TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += CCI.h

SOURCES += CCI.cpp

TARGET = CCI

INCLUDEPATH += ../../../lib

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
