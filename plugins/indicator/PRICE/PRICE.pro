TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += PRICE.h

SOURCES += PRICE.cpp

TARGET = PRICE

INCLUDEPATH += ../../../lib

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
