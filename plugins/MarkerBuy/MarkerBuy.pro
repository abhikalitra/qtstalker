TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += MarkerBuy.h
SOURCES += MarkerBuy.cpp
HEADERS += MarkerBuyDialog.h
SOURCES += MarkerBuyDialog.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
