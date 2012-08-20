include (../../plugin.pri)


TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib

MOC_DIR += build
OBJECTS_DIR += build

HEADERS += ADX.h
SOURCES += ADX.cpp
HEADERS += ADXDialog.h
SOURCES += ADXDialog.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
