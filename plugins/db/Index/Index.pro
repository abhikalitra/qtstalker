TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += Index.h
HEADERS += IndexDialog.h

SOURCES += Index.cpp
SOURCES += IndexDialog.cpp

TARGET = Index.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/db
INSTALLS += target
