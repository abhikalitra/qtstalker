TEMPLATE = lib

CONFIG += thread warn_on debug plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Plugin.h
HEADERS += IndicatorPlugin.h
HEADERS += Setting.h
HEADERS += PlotLine.h
HEADERS += DPO.h

SOURCES += Plugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += Setting.cpp
SOURCES += PlotLine.cpp
SOURCES += DPO.cpp

TARGET = DPO

VERSION = 0.2

LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

