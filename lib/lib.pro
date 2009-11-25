TEMPLATE = lib

CONFIG += thread warn_on debug
CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += Setting.h
HEADERS += PlotLine.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += PrefDialog.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
HEADERS += Bar.h
HEADERS += Config.h
HEADERS += Plot.h
HEADERS += DatePlot.h
HEADERS += ScalePlot.h
HEADERS += IndicatorPlot.h
HEADERS += DataWindow.h
HEADERS += Preferences.h
HEADERS += DataBase.h
HEADERS += Setup.h
HEADERS += ChartPage.h
HEADERS += GroupPage.h
HEADERS += IndicatorPage.h
HEADERS += COSettings.h
HEADERS += ExScript.h
HEADERS += Indicator.h
HEADERS += CODialog.h
HEADERS += IndicatorSettings.h
HEADERS += IndicatorDialog.h
HEADERS += IndicatorScript.h



SOURCES += Setting.cpp
SOURCES += PlotLine.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += PrefDialog.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
SOURCES += Bar.cpp
SOURCES += Config.cpp
SOURCES += Plot.cpp
SOURCES += DatePlot.cpp
SOURCES += ScalePlot.cpp
SOURCES += IndicatorPlot.cpp
SOURCES += DataWindow.cpp
SOURCES += Preferences.cpp
SOURCES += DataBase.cpp
SOURCES += Setup.cpp
SOURCES += ChartPage.cpp
SOURCES += GroupPage.cpp
SOURCES += IndicatorPage.cpp
SOURCES += COSettings.cpp
SOURCES += ExScript.cpp
SOURCES += Indicator.cpp
SOURCES += CODialog.cpp
SOURCES += IndicatorSettings.cpp
SOURCES += IndicatorDialog.cpp
SOURCES += IndicatorScript.cpp



TARGET = qtstalker

VERSION = 0.37.0

unix {
  # qmake automatically adds /usr/lib
  INCLUDEPATH += /usr/include/qt4/Qt
  exists(/usr/local/lib) {
    LIBS += -L/usr/local/lib
  }
}

TA_LIB_VERSION = $$system(ta-lib-config --version)
contains(TA_LIB_VERSION, 0.3.0) {
    LIBS += -lta_abstract
    LIBS += -lta_common
    LIBS += -lta_func
  } else {
    LIBS += -lta_lib
  }

unix:linux-g++:LIBS += -ldl

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_LIB_DIR}"
INSTALLS += target

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += qtstalker_defines.h

QT += sql



