TEMPLATE = lib

CONFIG += thread warn_on debug dll

HEADERS += Plugin.h
HEADERS += QuotePlugin.h
HEADERS += Setting.h
HEADERS += ChartDb.h
HEADERS += CME.h

SOURCES += Plugin.cpp
SOURCES += QuotePlugin.cpp
SOURCES += Setting.cpp
SOURCES += ChartDb.cpp
SOURCES += CME.cpp

TARGET = CME

VERSION = 0.2

INCLUDEPATH += ../../gdbm-1.8.0

LIBS += -L../../gdbm-1.8.0/.libs -lgdbm -ldl
LIBS += -L/usr/lib
