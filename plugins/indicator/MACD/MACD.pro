TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += MACD.h

SOURCES += MACD.cpp

TARGET = MACD

INCLUDEPATH += ../../../lib

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
