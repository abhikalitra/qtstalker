TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += MOM.h

SOURCES += MOM.cpp

TARGET = MOM

INCLUDEPATH += ../../../lib

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
