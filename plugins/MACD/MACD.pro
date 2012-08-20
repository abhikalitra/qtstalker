include (../../plugin.pri)

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += MACD.h
SOURCES += MACD.cpp
HEADERS += MACDDialog.h
SOURCES += MACDDialog.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
