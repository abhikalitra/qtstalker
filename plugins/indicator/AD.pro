TEMPLATE = lib

CONFIG += thread warn_on debug dll

VERSION = 0.2

HEADERS += Plugin.h
HEADERS += IndicatorPlugin.h
HEADERS += Setting.h
HEADERS += AD.h

SOURCES += Plugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += Setting.cpp
SOURCES += AD.cpp

TARGET = AD

LIBS += -ldl
LIBS += -L/usr/lib


