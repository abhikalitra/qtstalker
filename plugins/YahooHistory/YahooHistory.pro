TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += GUIWidget.h
SOURCES += GUIWidget.cpp
HEADERS += YahooHistory.h
SOURCES += YahooHistory.cpp
HEADERS += YahooHistoryDownload.h
SOURCES += YahooHistoryDownload.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
