TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += Bar.h
HEADERS += BarData.h
HEADERS += BarRange.h
HEADERS += ColorButton.h
HEADERS += Command.h
HEADERS += CommandThread.h
HEADERS += Curve.h
HEADERS += CurveBar.h
HEADERS += Doc.h
HEADERS += FontButton.h
HEADERS += FunctionMA.h
HEADERS += Globals.h
HEADERS += Indicator.h
HEADERS += Message.h
HEADERS += Operator.h
HEADERS += ScriptPlugin.h
HEADERS += ScriptPluginFactory.h
HEADERS += Setting.h
HEADERS += Strip.h


SOURCES += Bar.cpp
SOURCES += BarData.cpp
SOURCES += BarRange.cpp
SOURCES += ColorButton.cpp
SOURCES += Command.cpp
SOURCES += CommandThread.cpp
SOURCES += Curve.cpp
SOURCES += CurveBar.cpp
SOURCES += Doc.cpp
SOURCES += FontButton.cpp
SOURCES += FunctionMA.cpp
SOURCES += Globals.cpp
SOURCES += Indicator.cpp
SOURCES += Message.cpp
SOURCES += Operator.cpp
SOURCES += ScriptPlugin.cpp
SOURCES += ScriptPluginFactory.cpp
SOURCES += Setting.cpp
SOURCES += Strip.cpp


TARGET = qtstalker

VERSION = 0.37.0

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_LIB_DIR}"
INSTALLS += target

QMAKE_DISTCLEAN += qtstalker_defines.h
QMAKE_DISTCLEAN += ../.qmake.cache

QT += core
QT += gui
QT += sql
QT += network

