TEMPLATE = lib

CONFIG += thread warn_on debug qdbus
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

# headers
HEADERS += BarLength.h

HEADERS += CandleType.h
HEADERS += ChartObjectData.h
HEADERS += ChartObjectDataBase.h
HEADERS += ChartObjectType.h
HEADERS += ColorButton.h
HEADERS += Command.h
HEADERS += CommandAD.h
HEADERS += CommandADX.h
HEADERS += CommandAdaptor.h
HEADERS += CommandArithmetic.h
HEADERS += CommandAROON.h
HEADERS += CommandATR.h
HEADERS += CommandAveragePrice.h
HEADERS += CommandBBANDS.h
HEADERS += CommandBETA.h
HEADERS += CommandBOP.h
HEADERS += CommandBuy.h
HEADERS += CommandCandlePattern.h
HEADERS += CommandCCI.h
HEADERS += CommandChart.h
HEADERS += CommandChartPanelRefresh.h
HEADERS += CommandChartUpdate.h
HEADERS += CommandCMO.h
HEADERS += CommandColor.h
HEADERS += CommandCompare.h
HEADERS += CommandCORREL.h
HEADERS += CommandCSV.h
HEADERS += CommandExchangeSearchDialog.h
HEADERS += CommandFactory.h
HEADERS += CommandInterface.h
HEADERS += CommandFI.h
HEADERS += CommandFileDialog.h
HEADERS += CommandHLine.h
HEADERS += CommandHT.h
HEADERS += CommandLINEARREG.h
HEADERS += CommandMA.h
HEADERS += CommandMACD.h
HEADERS += CommandMAMA.h
HEADERS += CommandMedianPrice.h
HEADERS += CommandMINMAX.h
HEADERS += CommandMFI.h
HEADERS += CommandMOM.h
HEADERS += CommandNormalize.h
HEADERS += CommandPHASOR.h
HEADERS += CommandPO.h
HEADERS += CommandPlotLine.h
HEADERS += CommandPlotHistogram.h
HEADERS += CommandPlotOHLC.h
HEADERS += CommandRetracement.h
HEADERS += CommandROC.h
HEADERS += CommandRSI.h
HEADERS += CommandSAR.h
HEADERS += CommandScriptDone.h
HEADERS += CommandScriptStart.h
HEADERS += CommandSelectDialog.h
HEADERS += CommandSell.h
HEADERS += CommandShift.h
HEADERS += CommandSINE.h
HEADERS += CommandSTDDEV.h
HEADERS += CommandStochFast.h
HEADERS += CommandStochRSI.h
HEADERS += CommandStochSlow.h
HEADERS += CommandSZ.h
HEADERS += CommandSymbol.h
HEADERS += CommandSymbolCurrent.h
HEADERS += CommandSymbolDialog.h
HEADERS += CommandT3.h
HEADERS += CommandText.h
HEADERS += CommandTHERM.h
HEADERS += CommandTLine.h
HEADERS += CommandTRIX.h
HEADERS += CommandTypicalPrice.h
HEADERS += CommandULTOSC.h
HEADERS += CommandVAR.h
HEADERS += CommandVBP.h
HEADERS += CommandVFI.h
HEADERS += CommandVIDYA.h
HEADERS += CommandVLine.h
HEADERS += CommandWILLR.h
HEADERS += CommandWeightedClose.h
HEADERS += CommandYahooHistory.h
HEADERS += ConfirmDialog.h
HEADERS += CurveBar.h
HEADERS += CurveData.h
HEADERS += CurveType.h

HEADERS += Data.h
HEADERS += DataBase.h
HEADERS += DataDialog.h
HEADERS += DataFactory.h
HEADERS += DateRange.h
HEADERS += Dialog.h
HEADERS += Doc.h

HEADERS += ExchangeDataBase.h
HEADERS += ExchangeSearchDialog.h

HEADERS += FileButton.h
HEADERS += FontButton.h

HEADERS += GroupDataBase.h
HEADERS += GroupEditDialog.h

HEADERS += HistogramStyle.h

HEADERS += IconButton.h
HEADERS += InputType.h
HEADERS += IPCMessage.h

HEADERS += LineEdit.h
HEADERS += LineStyle.h

HEADERS += MAType.h
HEADERS += Message.h
HEADERS += MessageDialog.h
HEADERS += MessageSend.h

HEADERS += NewDialog.h

HEADERS += OHLCStyle.h
HEADERS += Operator.h

HEADERS += QuoteDataBase.h

HEADERS += Script.h
HEADERS += ScriptDataBase.h
HEADERS += SelectDialog.h
HEADERS += SharedMemory.h
HEADERS += Strip.h
HEADERS += Symbol.h
HEADERS += SymbolButton.h
HEADERS += SymbolDialog.h

#sources

SOURCES += BarLength.cpp

SOURCES += CandleType.cpp
SOURCES += ChartObjectData.cpp
SOURCES += ChartObjectDataBase.cpp
SOURCES += ChartObjectType.cpp
SOURCES += ColorButton.cpp
SOURCES += Command.cpp
SOURCES += CommandAD.cpp
SOURCES += CommandADX.cpp
SOURCES += CommandAdaptor.cpp
SOURCES += CommandArithmetic.cpp
SOURCES += CommandAROON.cpp
SOURCES += CommandATR.cpp
SOURCES += CommandAveragePrice.cpp
SOURCES += CommandBBANDS.cpp
SOURCES += CommandBETA.cpp
SOURCES += CommandBOP.cpp
SOURCES += CommandBuy.cpp
SOURCES += CommandCandlePattern.cpp
SOURCES += CommandCCI.cpp
SOURCES += CommandChart.cpp
SOURCES += CommandChartPanelRefresh.cpp
SOURCES += CommandChartUpdate.cpp
SOURCES += CommandCMO.cpp
SOURCES += CommandColor.cpp
SOURCES += CommandCompare.cpp
SOURCES += CommandCORREL.cpp
SOURCES += CommandCSV.cpp
SOURCES += CommandExchangeSearchDialog.cpp
SOURCES += CommandFactory.cpp
SOURCES += CommandInterface.cpp
SOURCES += CommandFI.cpp
SOURCES += CommandFileDialog.cpp
SOURCES += CommandHLine.cpp
SOURCES += CommandHT.cpp
SOURCES += CommandLINEARREG.cpp
SOURCES += CommandMA.cpp
SOURCES += CommandMACD.cpp
SOURCES += CommandMAMA.cpp
SOURCES += CommandMedianPrice.cpp
SOURCES += CommandMINMAX.cpp
SOURCES += CommandMFI.cpp
SOURCES += CommandMOM.cpp
SOURCES += CommandNormalize.cpp
SOURCES += CommandPO.cpp
SOURCES += CommandPHASOR.cpp
SOURCES += CommandPlotLine.cpp
SOURCES += CommandPlotHistogram.cpp
SOURCES += CommandPlotOHLC.cpp
SOURCES += CommandRetracement.cpp
SOURCES += CommandROC.cpp
SOURCES += CommandRSI.cpp
SOURCES += CommandSAR.cpp
SOURCES += CommandScriptDone.cpp
SOURCES += CommandScriptStart.cpp
SOURCES += CommandSelectDialog.cpp
SOURCES += CommandSell.cpp
SOURCES += CommandShift.cpp
SOURCES += CommandSINE.cpp
SOURCES += CommandSTDDEV.cpp
SOURCES += CommandStochFast.cpp
SOURCES += CommandStochRSI.cpp
SOURCES += CommandStochSlow.cpp
SOURCES += CommandSZ.cpp
SOURCES += CommandSymbol.cpp
SOURCES += CommandSymbolCurrent.cpp
SOURCES += CommandSymbolDialog.cpp
SOURCES += CommandT3.cpp
SOURCES += CommandText.cpp
SOURCES += CommandTHERM.cpp
SOURCES += CommandTLine.cpp
SOURCES += CommandTRIX.cpp
SOURCES += CommandTypicalPrice.cpp
SOURCES += CommandULTOSC.cpp
SOURCES += CommandVAR.cpp
SOURCES += CommandVBP.cpp
SOURCES += CommandVFI.cpp
SOURCES += CommandVIDYA.cpp
SOURCES += CommandVLine.cpp
SOURCES += CommandWILLR.cpp
SOURCES += CommandWeightedClose.cpp
SOURCES += CommandYahooHistory.cpp
SOURCES += ConfirmDialog.cpp
SOURCES += CurveBar.cpp
SOURCES += CurveData.cpp
SOURCES += CurveType.cpp

SOURCES += Data.cpp
SOURCES += DataBase.cpp
SOURCES += DataDialog.cpp
SOURCES += DataFactory.cpp
SOURCES += DateRange.cpp
SOURCES += Dialog.cpp
SOURCES += Doc.cpp

SOURCES += ExchangeDataBase.cpp
SOURCES += ExchangeSearchDialog.cpp

SOURCES += FileButton.cpp
SOURCES += FontButton.cpp

SOURCES += GroupDataBase.cpp
SOURCES += GroupEditDialog.cpp

SOURCES += HistogramStyle.cpp

SOURCES += IconButton.cpp
SOURCES += InputType.cpp
SOURCES += IPCMessage.cpp

SOURCES += LineEdit.cpp
SOURCES += LineStyle.cpp

SOURCES += MAType.cpp
SOURCES += MessageDialog.cpp
SOURCES += MessageSend.cpp

SOURCES += NewDialog.cpp

SOURCES += OHLCStyle.cpp
SOURCES += Operator.cpp

SOURCES += QuoteDataBase.cpp

SOURCES += Script.cpp
SOURCES += ScriptDataBase.cpp
SOURCES += SelectDialog.cpp
SOURCES += SharedMemory.cpp
SOURCES += Strip.cpp
SOURCES += Symbol.cpp
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
QT += dbus
