TEMPLATE = lib

CONFIG += thread warn_on debug plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Plugin.h
HEADERS += QuotePlugin.h
HEADERS += Setting.h
HEADERS += ChartDb.h
HEADERS += Ratio.h

SOURCES += Plugin.cpp
SOURCES += QuotePlugin.cpp
SOURCES += Setting.cpp
SOURCES += ChartDb.cpp
SOURCES += Ratio.cpp

TARGET = Ratio

VERSION = 0.2

INCLUDEPATH += ../../db-2.7.7/build_unix

LIBS += -L../../db-2.7.7/build_unix -ldb
LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle
