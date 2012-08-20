include (../../plugin.pri)

TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += ROC.h
SOURCES += ROC.cpp
HEADERS += ROCDialog.h
SOURCES += ROCDialog.cpp
HEADERS += ROCType.h
SOURCES += ROCType.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
