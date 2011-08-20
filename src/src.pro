TEMPLATE = app

CONFIG += qt thread warn_on debug qdbus
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

#headers
HEADERS += Qtstalker.h
HEADERS += Setup.h

#sources
SOURCES += main.cpp
SOURCES += Qtstalker.cpp
SOURCES += Setup.cpp


TARGET = qtstalker

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
