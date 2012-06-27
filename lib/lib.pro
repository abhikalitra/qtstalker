######################################################
# SOURCE
#####################################################

HEADERS += Bar.h
SOURCES += Bar.cpp
HEADERS += Bars.h
SOURCES += Bars.cpp
HEADERS += BarLength.h
SOURCES += BarLength.cpp
HEADERS += BarType.h
SOURCES += BarType.cpp
HEADERS += CBar.h
SOURCES += CBar.cpp
HEADERS += ColorButton.h
SOURCES += ColorButton.cpp
HEADERS += Curve.h
SOURCES += Curve.cpp
HEADERS += CurveHistogramType.h
SOURCES += CurveHistogramType.cpp
HEADERS += CurveLineType.h
SOURCES += CurveLineType.cpp
HEADERS += CurveOHLCType.h
SOURCES += CurveOHLCType.cpp
HEADERS += DataBase.h
SOURCES += DataBase.cpp
HEADERS += DateRange.h
SOURCES += DateRange.cpp
HEADERS += Dialog.h
SOURCES += Dialog.cpp
HEADERS += Doc.h
SOURCES += Doc.cpp
HEADERS += Entity.h
SOURCES += Entity.cpp
HEADERS += FileButton.h
SOURCES += FileButton.cpp
HEADERS += FontButton.h
SOURCES += FontButton.cpp
HEADERS += Global.h
SOURCES += Global.cpp
HEADERS += MAType.h
SOURCES += MAType.cpp
HEADERS += Marker.h
SOURCES += Marker.cpp
HEADERS += PickerMachine.h
SOURCES += PickerMachine.cpp
HEADERS += Plot.h
SOURCES += Plot.cpp
HEADERS += PlotDateScaleDraw.h
SOURCES += PlotDateScaleDraw.cpp
HEADERS += PlotInfo.h
SOURCES += PlotInfo.cpp
HEADERS += PlotPicker.h
SOURCES += PlotPicker.cpp
HEADERS += PlotScaleDraw.h
SOURCES += PlotScaleDraw.cpp
HEADERS += PlotSettings.h
HEADERS += PlotStatus.h
HEADERS += Plugin.h
HEADERS += PluginData.h
HEADERS += PluginFactory.h
SOURCES += PluginFactory.cpp
HEADERS += Strip.h
SOURCES += Strip.cpp
HEADERS += Widget.h
SOURCES += Widget.cpp

######################################################
# QMAKE SETTINGS
#####################################################

TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

TARGET = OTA

VERSION = 0.1.0

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_LIB_DIR}"
INSTALLS += target

QMAKE_DISTCLEAN += qtstalker_defines.h
QMAKE_DISTCLEAN += ../.qmake.cache

MOC_DIR += build
OBJECTS_DIR += build

QT += core
QT += gui
QT += sql
QT += network
