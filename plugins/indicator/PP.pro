TEMPLATE = lib

CONFIG += thread warn_on debug dll
QMAKE_CXXFLAGS += -O3

HEADERS += Plugin.h
HEADERS += IndicatorPlugin.h
HEADERS += Setting.h
HEADERS += PlotLine.h
HEADERS += PP.h

SOURCES += Plugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += Setting.cpp
SOURCES += PlotLine.cpp
SOURCES += PP.cpp

TARGET = PP

VERSION = 0.2

LIBS += -ldl
LIBS += -L/usr/lib


