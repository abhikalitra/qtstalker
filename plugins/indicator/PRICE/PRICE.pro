TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += ../../../src/Plugin.h
HEADERS += ../../../src/IndicatorPlugin.h
HEADERS += ../../../src/Setting.h
HEADERS += ../../../src/PlotLine.h
HEADERS += PRICE.h

SOURCES += ../../../src/Plugin.cpp
SOURCES += ../../../src/IndicatorPlugin.cpp
SOURCES += ../../../src/Setting.cpp
SOURCES += ../../../src/PlotLine.cpp
SOURCES += PRICE.cpp

TARGET = PRICE

INCLUDEPATH += ../../../src

LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

unix:OBJECTS_DIR = ../../../src
unix:MOC_DIR = ../../../src

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target