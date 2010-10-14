TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += Bar.h
HEADERS += BarData.h
HEADERS += BarRange.h
HEADERS += ChartObject.h
HEADERS += ChartObjectBuy.h
HEADERS += ChartObjectBuyDialog.h
HEADERS += ChartObjectBuyDraw.h
HEADERS += ChartObjectDataBase.h
HEADERS += ChartObjectDialog.h
HEADERS += ChartObjectDraw.h
HEADERS += ChartObjectFactory.h
HEADERS += ChartObjectHLine.h
HEADERS += ChartObjectHLineDialog.h
HEADERS += ChartObjectHLineDraw.h
HEADERS += ChartObjectRetracement.h
HEADERS += ChartObjectRetracementDialog.h
HEADERS += ChartObjectRetracementDraw.h
HEADERS += ChartObjectSell.h
HEADERS += ChartObjectSellDialog.h
HEADERS += ChartObjectSellDraw.h
HEADERS += ChartObjectSettings.h
HEADERS += ChartObjectText.h
HEADERS += ChartObjectTextDialog.h
HEADERS += ChartObjectTextDraw.h
HEADERS += ChartObjectTLine.h
HEADERS += ChartObjectTLineDialog.h
HEADERS += ChartObjectTLineDraw.h
HEADERS += ChartObjectVLine.h
HEADERS += ChartObjectVLineDialog.h
HEADERS += ChartObjectVLineDraw.h
HEADERS += ColorButton.h
HEADERS += Config.h
HEADERS += ConfigDataBase.h
HEADERS += Curve.h
HEADERS += CurveBar.h
HEADERS += DataBase.h
HEADERS += DateRange.h
HEADERS += DateScaleDraw.h
HEADERS += Dialog.h
HEADERS += ExchangeDataBase.h
HEADERS += ExchangeSearchDialog.h
HEADERS += ExScript.h
HEADERS += FileButton.h
HEADERS += FontButton.h
HEADERS += FunctionBARS.h
HEADERS += FunctionMA.h
HEADERS += FunctionNORMALIZE.h
HEADERS += FuturesDataBase.h
HEADERS += Globals.h
HEADERS += Group.h
HEADERS += GroupDataBase.h
HEADERS += Indicator.h
HEADERS += IndicatorDataBase.h
HEADERS += IndicatorPlotList.h
HEADERS += IndicatorPlotRule.h
HEADERS += IndicatorPlotRules.h
HEADERS += IndicatorPlugin.h
HEADERS += IndicatorPluginDialog.h
HEADERS += IndicatorPluginFactory.h
HEADERS += IndicatorThread.h
HEADERS += ListDialog.h
HEADERS += MiscPlugin.h
HEADERS += MiscPluginFactory.h
HEADERS += NewDialog.h
HEADERS += OHLC.h
HEADERS += Operator.h
HEADERS += PickerMachine.h
HEADERS += Plot.h
HEADERS += PlotCandle.h
HEADERS += PlotHistogramBar.h
HEADERS += PlotOHLC.h
HEADERS += PlotPicker.h
HEADERS += PlotSettings.h
HEADERS += PlotScaleDraw.h
HEADERS += PluginFactory.h
HEADERS += QuoteServerRequest.h
HEADERS += QuoteServerRequestThread.h
HEADERS += SCChartObject.h
HEADERS += SCGroup.h
HEADERS += SCIndicator.h
HEADERS += SCProcess.h
HEADERS += SCSymbol.h
HEADERS += Script.h
HEADERS += ScriptDataBase.h
HEADERS += Setting.h
HEADERS += Setup.h
HEADERS += Strip.h
HEADERS += SymbolButton.h
HEADERS += SymbolDialog.h


SOURCES += Bar.cpp
SOURCES += BarData.cpp
SOURCES += BarRange.cpp
SOURCES += ChartObject.cpp
SOURCES += ChartObjectBuy.cpp
SOURCES += ChartObjectBuyDialog.cpp
SOURCES += ChartObjectBuyDraw.cpp
SOURCES += ChartObjectDataBase.cpp
SOURCES += ChartObjectDialog.cpp
SOURCES += ChartObjectDraw.cpp
SOURCES += ChartObjectFactory.cpp
SOURCES += ChartObjectHLine.cpp
SOURCES += ChartObjectHLineDialog.cpp
SOURCES += ChartObjectHLineDraw.cpp
SOURCES += ChartObjectRetracement.cpp
SOURCES += ChartObjectRetracementDialog.cpp
SOURCES += ChartObjectRetracementDraw.cpp
SOURCES += ChartObjectSell.cpp
SOURCES += ChartObjectSellDialog.cpp
SOURCES += ChartObjectSellDraw.cpp
SOURCES += ChartObjectText.cpp
SOURCES += ChartObjectTextDialog.cpp
SOURCES += ChartObjectTextDraw.cpp
SOURCES += ChartObjectTLine.cpp
SOURCES += ChartObjectTLineDialog.cpp
SOURCES += ChartObjectTLineDraw.cpp
SOURCES += ChartObjectVLine.cpp
SOURCES += ChartObjectVLineDialog.cpp
SOURCES += ChartObjectVLineDraw.cpp
SOURCES += ColorButton.cpp
SOURCES += Config.cpp
SOURCES += ConfigDataBase.cpp
SOURCES += Curve.cpp
SOURCES += CurveBar.cpp
SOURCES += DataBase.cpp
SOURCES += DateRange.cpp
SOURCES += DateScaleDraw.cpp
SOURCES += Dialog.cpp
SOURCES += ExchangeDataBase.cpp
SOURCES += ExchangeSearchDialog.cpp
SOURCES += ExScript.cpp
SOURCES += FileButton.cpp
SOURCES += FontButton.cpp
SOURCES += FunctionBARS.cpp
SOURCES += FunctionMA.cpp
SOURCES += FunctionNORMALIZE.cpp
SOURCES += FuturesDataBase.cpp
SOURCES += Globals.cpp
SOURCES += Group.cpp
SOURCES += GroupDataBase.cpp
SOURCES += Indicator.cpp
SOURCES += IndicatorDataBase.cpp
SOURCES += IndicatorPlotList.cpp
SOURCES += IndicatorPlotRule.cpp
SOURCES += IndicatorPlotRules.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += IndicatorPluginDialog.cpp
SOURCES += IndicatorPluginFactory.cpp
SOURCES += IndicatorThread.cpp
SOURCES += ListDialog.cpp
SOURCES += MiscPlugin.cpp
SOURCES += MiscPluginFactory.cpp
SOURCES += NewDialog.cpp
SOURCES += Operator.cpp
SOURCES += PickerMachine.cpp
SOURCES += Plot.cpp
SOURCES += PlotCandle.cpp
SOURCES += PlotHistogramBar.cpp
SOURCES += PlotOHLC.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp
SOURCES += PluginFactory.cpp
SOURCES += QuoteServerRequest.cpp
SOURCES += QuoteServerRequestThread.cpp
SOURCES += SCChartObject.cpp
SOURCES += SCGroup.cpp
SOURCES += SCIndicator.cpp
SOURCES += SCProcess.cpp
SOURCES += SCSymbol.cpp
SOURCES += Script.cpp
SOURCES += ScriptDataBase.cpp
SOURCES += Setting.cpp
SOURCES += Setup.cpp
SOURCES += Strip.cpp
SOURCES += SymbolButton.cpp
SOURCES += SymbolDialog.cpp


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

