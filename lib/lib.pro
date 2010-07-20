TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2


HEADERS += QuoteServerRequestThread.h
HEADERS += QuoteServerRequest.h
HEADERS += Globals.h
HEADERS += Setting.h
HEADERS += PlotLine.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += PrefDialog.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
HEADERS += Bar.h
HEADERS += BaseConfig.h
HEADERS += Config.h
HEADERS += Plot.h
HEADERS += DatePlot.h
HEADERS += ScalePlot.h
HEADERS += Setup.h
HEADERS += ExScript.h
HEADERS += Indicator.h
HEADERS += SCIndicator.h
HEADERS += SCPlot.h
HEADERS += SCGroup.h
HEADERS += SCTest.h
HEADERS += Script.h
HEADERS += IndicatorPlugin.h
HEADERS += PluginFactory.h
HEADERS += COPlugin.h
HEADERS += PlotLineBar.h
HEADERS += SymbolButton.h
HEADERS += SymbolDialog.h
HEADERS += DateBar.h
HEADERS += PlotGrid.h
HEADERS += PlotCursorInfo.h
HEADERS += GroupDataBase.h
HEADERS += Group.h
HEADERS += ScriptDataBase.h
HEADERS += IndicatorDataBase.h
HEADERS += CODataBase.h
HEADERS += FileButton.h
HEADERS += ExchangeDataBase.h
HEADERS += FuturesDataBase.h
HEADERS += Buy.h
HEADERS += HLine.h
HEADERS += Retracement.h
HEADERS += Sell.h
HEADERS += Text.h
HEADERS += TLine.h
HEADERS += VLine.h
HEADERS += PlotData.h
HEADERS += COFactory.h
HEADERS += PlotFactory.h
HEADERS += Candle.h
HEADERS += Dash.h
HEADERS += Dot.h
HEADERS += Histogram.h
HEADERS += HistogramBar.h
HEADERS += Horizontal.h
HEADERS += Line.h
HEADERS += OHLC.h
HEADERS += DataBase.h
HEADERS += SCSymbol.h
HEADERS += BARSUtils.h
HEADERS += NORMALIZE.h
HEADERS += MAFactory.h
HEADERS += Wilder.h
HEADERS += Strip.h
HEADERS += PlotDrawInfo.h
HEADERS += SCChartObject.h
HEADERS += IndicatorPluginFactory.h
HEADERS += MiscPluginFactory.h
HEADERS += MiscPlugin.h
HEADERS += PlotCursorFactory.h
HEADERS += PlotCursor.h
HEADERS += PlotCursorZoom.h
HEADERS += PlotCursorCrossHair.h
HEADERS += PlotCursorNormal.h
HEADERS += DateRange.h
HEADERS += DataDataBase.h
HEADERS += IndicatorThread.h
HEADERS += BarRange.h


SOURCES += QuoteServerRequestThread.cpp
SOURCES += QuoteServerRequest.cpp
SOURCES += Globals.cpp
SOURCES += Setting.cpp
SOURCES += PlotLine.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += PrefDialog.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
SOURCES += Bar.cpp
SOURCES += BaseConfig.cpp
SOURCES += Config.cpp
SOURCES += Plot.cpp
SOURCES += DatePlot.cpp
SOURCES += ScalePlot.cpp
SOURCES += Setup.cpp
SOURCES += ExScript.cpp
SOURCES += Indicator.cpp
SOURCES += SCIndicator.cpp
SOURCES += SCPlot.cpp
SOURCES += SCGroup.cpp
SOURCES += SCTest.cpp
SOURCES += Script.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += PluginFactory.cpp
SOURCES += COPlugin.cpp
SOURCES += PlotLineBar.cpp
SOURCES += SymbolButton.cpp
SOURCES += SymbolDialog.cpp
SOURCES += DateBar.cpp
SOURCES += PlotGrid.cpp
SOURCES += PlotCursorInfo.cpp
SOURCES += GroupDataBase.cpp
SOURCES += Group.cpp
SOURCES += ScriptDataBase.cpp
SOURCES += IndicatorDataBase.cpp
SOURCES += CODataBase.cpp
SOURCES += FileButton.cpp
SOURCES += ExchangeDataBase.cpp
SOURCES += FuturesDataBase.cpp
SOURCES += Buy.cpp
SOURCES += HLine.cpp
SOURCES += Retracement.cpp
SOURCES += Sell.cpp
SOURCES += Text.cpp
SOURCES += TLine.cpp
SOURCES += VLine.cpp
SOURCES += COFactory.cpp
SOURCES += PlotFactory.cpp
SOURCES += Candle.cpp
SOURCES += Dash.cpp
SOURCES += Dot.cpp
SOURCES += Histogram.cpp
SOURCES += HistogramBar.cpp
SOURCES += Horizontal.cpp
SOURCES += Line.cpp
SOURCES += OHLC.cpp
SOURCES += DataBase.cpp
SOURCES += SCSymbol.cpp
SOURCES += BARSUtils.cpp
SOURCES += NORMALIZE.cpp
SOURCES += MAFactory.cpp
SOURCES += Wilder.cpp
SOURCES += Strip.cpp
SOURCES += PlotDrawInfo.cpp
SOURCES += SCChartObject.cpp
SOURCES += IndicatorPluginFactory.cpp
SOURCES += MiscPluginFactory.cpp
SOURCES += MiscPlugin.cpp
SOURCES += PlotCursorFactory.cpp
SOURCES += PlotCursor.cpp
SOURCES += PlotCursorZoom.cpp
SOURCES += PlotCursorCrossHair.cpp
SOURCES += PlotCursorNormal.cpp
SOURCES += DateRange.cpp
SOURCES += DataDataBase.cpp
SOURCES += IndicatorThread.cpp
SOURCES += BarRange.cpp


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

#unix:linux-g++:LIBS += -ldl

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_LIB_DIR}"
INSTALLS += target

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += qtstalker_defines.h

QT += core
QT += gui
QT += sql
QT += network

