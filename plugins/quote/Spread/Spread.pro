TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += ../../../src/Plugin.h
HEADERS += ../../../src/QuotePlugin.h
HEADERS += ../../../src/Setting.h
HEADERS += ../../../src/ChartDb.h
HEADERS += Spread.h

SOURCES += ../../../src/Plugin.cpp
SOURCES += ../../../src/QuotePlugin.cpp
SOURCES += ../../../src/Setting.cpp
SOURCES += ../../../src/ChartDb.cpp
SOURCES += Spread.cpp

TARGET = Spread

VERSION = 0.2

INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -L../../../db-2.7.7/build_unix -ldb
LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

unix:OBJECTS_DIR = ../../../src

target.path = /usr/lib/qtstalker/quote
INSTALLS += target