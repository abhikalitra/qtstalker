TEMPLATE = lib

CONFIG += thread warn_on debug dll

HEADERS += Plugin.h
HEADERS += IndicatorPlugin.h
HEADERS += Setting.h
HEADERS += OI.h

SOURCES += Plugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += Setting.cpp
SOURCES += OI.cpp

TARGET = OI

VERSION = 0.2

LIBS += -ldl
LIBS += -L/usr/lib


