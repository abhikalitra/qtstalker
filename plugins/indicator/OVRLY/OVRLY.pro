TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += OVRLY.h

SOURCES += OVRLY.cpp

TARGET = OVRLY

INCLUDEPATH += ../../../db-2.7.7/build_unix
INCLUDEPATH += ../../../lib

LIBS += -L../../../db-2.7.7/build_unix -ldb
LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
