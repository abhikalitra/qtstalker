TEMPLATE = lib

CONFIG += thread warn_on plugin
#QMAKE_CXXFLAGS += -Os

HEADERS += Bars.h

SOURCES += Bars.cpp

TARGET = Bar

INCLUDEPATH += ../../../lib

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
