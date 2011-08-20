TEMPLATE = app

CONFIG += qt thread warn_on debug qdbus
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += QtStalkerScript.h

SOURCES += QtStalkerScript.cpp
SOURCES += main.cpp

TARGET = QtStalkerScript

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
QT += dbus

