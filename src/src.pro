include (../config.pri)

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

CONFIG += qt thread warn_on
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

!win32 {
QMAKE_CXXFLAGS += -rdynamic -ffast-math
}

INCLUDEPATH += ../lib

LIBS += -L$${OTA_LIB1} -lOTA

TARGET = ota

message("src Using INCLUDEPATH=$$INCLUDEPATH")
message("src Using LIBS=$$LIBS")

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

MOC_DIR += build
OBJECTS_DIR += build

QT += core
QT += gui
QT += sql
QT += network
