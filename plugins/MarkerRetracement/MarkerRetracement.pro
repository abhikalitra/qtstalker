TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += MarkerRetracement.h
SOURCES += MarkerRetracement.cpp
HEADERS += MarkerRetracementDialog.h
SOURCES += MarkerRetracementDialog.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
