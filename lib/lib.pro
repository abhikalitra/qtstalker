TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += Bar.h
HEADERS += BarData.h
HEADERS += BarRange.h
HEADERS += ChartPage.h
HEADERS += ColorButton.h
HEADERS += Command.h
HEADERS += CommandThread.h
HEADERS += Curve.h
HEADERS += CurveBar.h
HEADERS += Doc.h
HEADERS += FontButton.h
HEADERS += FunctionMA.h
HEADERS += Globals.h
HEADERS += GroupPage.h
HEADERS += Indicator.h
HEADERS += Operator.h
HEADERS += Script.h
HEADERS += ScriptLaunchButton.h
HEADERS += ScriptPage.h
HEADERS += ScriptPlugin.h
HEADERS += ScriptPluginFactory.h
HEADERS += Setting.h
HEADERS += Strip.h
HEADERS += SymbolListWidget.h


SOURCES += Bar.cpp
SOURCES += BarData.cpp
SOURCES += BarRange.cpp
SOURCES += ChartPage.cpp
SOURCES += ColorButton.cpp
SOURCES += Command.cpp
SOURCES += CommandThread.cpp
SOURCES += Curve.cpp
SOURCES += CurveBar.cpp
SOURCES += Doc.cpp
SOURCES += FontButton.cpp
SOURCES += FunctionMA.cpp
SOURCES += Globals.cpp
SOURCES += GroupPage.cpp
SOURCES += Indicator.cpp
SOURCES += Operator.cpp
SOURCES += Script.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptPage.cpp
SOURCES += ScriptPlugin.cpp
SOURCES += ScriptPluginFactory.cpp
SOURCES += Setting.cpp
SOURCES += Strip.cpp
SOURCES += SymbolListWidget.cpp


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

