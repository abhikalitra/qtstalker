include (../../plugin.pri)

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += CDL.h
SOURCES += CDL.cpp
HEADERS += TypeCandle.h
SOURCES += TypeCandle.cpp
HEADERS += CDLDialog.h
SOURCES += CDLDialog.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
