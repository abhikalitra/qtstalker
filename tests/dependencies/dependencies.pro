!exists(../../.qmake.cache) {
  error("Please do 'configure' for Qtstalker before running tests")
}

QT -= gui

CONFIG += qtestlib
CONFIG -= app_bundle

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

LIBS += -ldb

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

# Input
SOURCES += Dependencies.cpp
