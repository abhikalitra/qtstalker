TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += CME.h

SOURCES += CME.cpp

TARGET = CME

#INCLUDEPATH += ../../../db-2.7.7/build_unix
INCLUDEPATH += ../../../lib

#LIBS += -L../../../db-2.7.7/build_unix -ldb
#LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
