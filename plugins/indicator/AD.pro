TEMPLATE = lib

CONFIG += thread warn_on debug plugin
QMAKE_CXXFLAGS += -Os

VERSION = 0.2

HEADERS += Plugin.h
HEADERS += IndicatorPlugin.h
HEADERS += Setting.h
HEADERS += PlotLine.h
HEADERS += AD.h

SOURCES += Plugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += Setting.cpp
SOURCES += PlotLine.cpp
SOURCES += AD.cpp

TARGET = AD

LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle


