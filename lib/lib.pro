TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += Bar.h
HEADERS += BarData.h
HEADERS += BarLength.h
HEADERS += ChartObjectDataBase.h
HEADERS += ColorButton.h
HEADERS += Command.h
HEADERS += CommandThread.h
HEADERS += ConfirmDialog.h
HEADERS += Curve.h
HEADERS += CurveBar.h
HEADERS += DateRange.h
HEADERS += Dialog.h
HEADERS += Doc.h
HEADERS += ExchangeDataBase.h
HEADERS += FileButton.h
HEADERS += FontButton.h
HEADERS += Globals.h
HEADERS += GroupDataBase.h
HEADERS += IconButton.h
HEADERS += Indicator.h
HEADERS += IndicatorDataBase.h
HEADERS += LineEdit.h
HEADERS += MiddleMan.h
HEADERS += Operator.h
HEADERS += QuoteDataBase.h
HEADERS += Script.h
HEADERS += ScriptDataBase.h
HEADERS += ScriptPlugin.h
HEADERS += ScriptPluginFactory.h
HEADERS += Setting.h
HEADERS += Strip.h
HEADERS += TestDataBase.h


SOURCES += Bar.cpp
SOURCES += BarData.cpp
SOURCES += BarLength.cpp
SOURCES += ChartObjectDataBase.cpp
SOURCES += ColorButton.cpp
SOURCES += Command.cpp
SOURCES += CommandThread.cpp
SOURCES += ConfirmDialog.cpp
SOURCES += Curve.cpp
SOURCES += CurveBar.cpp
SOURCES += DateRange.cpp
SOURCES += Dialog.cpp
SOURCES += Doc.cpp
SOURCES += ExchangeDataBase.cpp
SOURCES += FileButton.cpp
SOURCES += FontButton.cpp
SOURCES += Globals.cpp
SOURCES += GroupDataBase.cpp
SOURCES += IconButton.cpp
SOURCES += Indicator.cpp
SOURCES += IndicatorDataBase.cpp
SOURCES += LineEdit.cpp
SOURCES += MiddleMan.cpp
SOURCES += Operator.cpp
SOURCES += QuoteDataBase.cpp
SOURCES += Script.cpp
SOURCES += ScriptDataBase.cpp
SOURCES += ScriptPlugin.cpp
SOURCES += ScriptPluginFactory.cpp
SOURCES += Setting.cpp
SOURCES += Strip.cpp
SOURCES += TestDataBase.cpp


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

