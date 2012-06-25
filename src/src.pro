######################################################
# FILES
#####################################################

SOURCES += LaunchPluginDialog.cpp
HEADERS += LaunchPluginDialog.h
SOURCES += main.cpp
SOURCES += OTA.cpp
HEADERS += OTA.h
SOURCES += Setup.cpp
HEADERS += Setup.h

######################################################
# QMAKE SETTINGS
#####################################################

TEMPLATE = app

CONFIG += qt thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

INCLUDEPATH += ../lib

LIBS += -L../lib -lOTA

TARGET = ota

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

MOC_DIR += build
OBJECTS_DIR += build

QT += core
QT += gui
QT += sql
QT += network
