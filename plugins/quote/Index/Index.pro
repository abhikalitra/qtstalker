TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Index.h
HEADERS += IndexDialog.h

SOURCES += Index.cpp
SOURCES += IndexDialog.cpp

TARGET = Index

INCLUDEPATH += ../../../lib
INCLUDEPATH += ../../../db-2.7.7/build_unix

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
