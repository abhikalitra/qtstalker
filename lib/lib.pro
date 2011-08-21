TEMPLATE = lib

CONFIG += thread warn_on debug qdbus
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

# headers
HEADERS += AboutDialog.h

HEADERS += Bar.h
HEADERS += BarData.h
HEADERS += BarLength.h
HEADERS += BarLengthButton.h
HEADERS += BarSpaceButton.h

HEADERS += CandleType.h
HEADERS += ChartObject.h
HEADERS += ChartObjectBuy.h
HEADERS += ChartObjectDataBase.h
HEADERS += ChartObjectFactory.h
HEADERS += ChartObjectHLine.h
HEADERS += ChartObjectRetracement.h
HEADERS += ChartObjectSell.h
HEADERS += ChartObjectText.h
HEADERS += ChartObjectTLine.h
HEADERS += ChartObjectVLine.h
HEADERS += ChartPage.h
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
HEADERS += CommandCandlePattern.h
HEADERS += CommandCCI.h
HEADERS += CommandChart.h
HEADERS += CommandChartUpdate.h
HEADERS += CommandCMO.h
HEADERS += CommandColor.h
HEADERS += CommandCompare.h
HEADERS += CommandCORREL.h
HEADERS += CommandCSV.h
HEADERS += CommandFactory.h
HEADERS += CommandInterface.h
HEADERS += CommandFI.h
HEADERS += CommandHT.h
HEADERS += CommandLINEARREG.h
HEADERS += CommandMA.h
HEADERS += CommandMACD.h
HEADERS += CommandMAMA.h
HEADERS += CommandMAVP.h
HEADERS += CommandMedianPrice.h
HEADERS += CommandMINMAX.h
HEADERS += CommandMFI.h
HEADERS += CommandMOM.h
HEADERS += CommandNormalize.h
HEADERS += CommandPHASOR.h
HEADERS += CommandPO.h
HEADERS += CommandPlotCandle.h
HEADERS += CommandPlotDash.h
HEADERS += CommandPlotDot.h
HEADERS += CommandPlotLine.h
HEADERS += CommandPlotHistogram.h
HEADERS += CommandPlotHistogramBar.h
HEADERS += CommandPlotOHLC.h
HEADERS += CommandROC.h
HEADERS += CommandRSI.h
HEADERS += CommandSAR.h
HEADERS += CommandScriptDebug.h
HEADERS += CommandScriptDone.h
HEADERS += CommandScriptStart.h
#HEADERS += CommandSelectDialog.h
HEADERS += CommandSINE.h
HEADERS += CommandSTDDEV.h
HEADERS += CommandStochFast.h
HEADERS += CommandStochRSI.h
HEADERS += CommandStochSlow.h
HEADERS += CommandSZ.h
HEADERS += CommandSymbol.h
HEADERS += CommandSymbolCurrent.h
HEADERS += CommandT3.h
HEADERS += CommandTHERM.h
HEADERS += CommandTRIX.h
HEADERS += CommandTypicalPrice.h
HEADERS += CommandULTOSC.h
HEADERS += CommandVAR.h
HEADERS += CommandVBP.h
HEADERS += CommandVFI.h
HEADERS += CommandVIDYA.h
HEADERS += CommandWILLR.h
HEADERS += CommandWidget.h
HEADERS += CommandWeightedClose.h
HEADERS += CommandYahooHistory.h
HEADERS += ConfigureButton.h
HEADERS += ConfigureDialog.h
HEADERS += ConfirmDialog.h
HEADERS += ControlPanel.h
HEADERS += CrossHairsButton.h
HEADERS += Curve.h
HEADERS += CurveBar.h
HEADERS += CurveCandle.h
HEADERS += CurveFactory.h
HEADERS += CurveHistogram.h
HEADERS += CurveLine.h
HEADERS += CurveOHLC.h

HEADERS += DataBase.h
HEADERS += DataWindowButton.h
HEADERS += DataWindow.h
HEADERS += DateRange.h
HEADERS += DateRangeControl.h
HEADERS += DateScaleDraw.h
HEADERS += Dialog.h
HEADERS += Doc.h
HEADERS += DockWidget.h

HEADERS += ExchangeDataBase.h

HEADERS += FileButton.h
HEADERS += FontButton.h

HEADERS += Globals.h
HEADERS += GridButton.h
HEADERS += GroupDataBase.h
HEADERS += GroupEditDialog.h
HEADERS += GroupPage.h

HEADERS += IconButton.h
HEADERS += IndicatorDialog.h
HEADERS += InfoPanel.h
HEADERS += InputType.h
HEADERS += IPCMessage.h

HEADERS += LineEdit.h

HEADERS += MAType.h
HEADERS += Message.h
HEADERS += MessageSend.h

HEADERS += NewDialog.h

HEADERS += Operator.h

HEADERS += PickerMachine.h
HEADERS += Plot.h
HEADERS += PlotGroup.h
HEADERS += PlotMenu.h
HEADERS += PlotPicker.h
HEADERS += PlotScaleDraw.h

HEADERS += QuoteDataBase.h

HEADERS += RecentCharts.h
HEADERS += RefreshButton.h

HEADERS += Script.h
HEADERS += ScriptDataBase.h
HEADERS += ScriptDialog.h
HEADERS += ScriptInsertDialog.h
HEADERS += ScriptLaunchButton.h
HEADERS += ScriptLaunchButtonDialog.h
HEADERS += ScriptPage.h
HEADERS += ScriptTimer.h
HEADERS += ScriptTimerDialog.h
HEADERS += SelectDialog.h
HEADERS += Setting.h
HEADERS += SettingBool.h
HEADERS += SettingColor.h
HEADERS += SettingDateTime.h
HEADERS += SettingDouble.h
HEADERS += SettingFont.h
HEADERS += SettingGroup.h
HEADERS += SettingInteger.h
HEADERS += SettingList.h
HEADERS += SettingString.h
HEADERS += SharedMemory.h
HEADERS += SidePanel.h
HEADERS += Strip.h
HEADERS += SymbolButton.h
HEADERS += SymbolDialog.h
HEADERS += SymbolListWidget.h

#sources

SOURCES += AboutDialog.cpp

SOURCES += Bar.cpp
SOURCES += BarData.cpp
SOURCES += BarLength.cpp
SOURCES += BarLengthButton.cpp
SOURCES += BarSpaceButton.cpp

SOURCES += CandleType.cpp
SOURCES += ChartObject.cpp
SOURCES += ChartObjectBuy.cpp
SOURCES += ChartObjectDataBase.cpp
SOURCES += ChartObjectFactory.cpp
SOURCES += ChartObjectHLine.cpp
SOURCES += ChartObjectRetracement.cpp
SOURCES += ChartObjectSell.cpp
SOURCES += ChartObjectText.cpp
SOURCES += ChartObjectTLine.cpp
SOURCES += ChartObjectVLine.cpp
SOURCES += ChartPage.cpp
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
SOURCES += CommandCandlePattern.cpp
SOURCES += CommandCCI.cpp
SOURCES += CommandChart.cpp
SOURCES += CommandChartUpdate.cpp
SOURCES += CommandCMO.cpp
SOURCES += CommandColor.cpp
SOURCES += CommandCompare.cpp
SOURCES += CommandCORREL.cpp
SOURCES += CommandCSV.cpp
SOURCES += CommandFactory.cpp
SOURCES += CommandInterface.cpp
SOURCES += CommandFI.cpp
SOURCES += CommandHT.cpp
SOURCES += CommandLINEARREG.cpp
SOURCES += CommandMA.cpp
SOURCES += CommandMACD.cpp
SOURCES += CommandMAMA.cpp
SOURCES += CommandMAVP.cpp
SOURCES += CommandMedianPrice.cpp
SOURCES += CommandMINMAX.cpp
SOURCES += CommandMFI.cpp
SOURCES += CommandMOM.cpp
SOURCES += CommandNormalize.cpp
SOURCES += CommandPO.cpp
SOURCES += CommandPHASOR.cpp
SOURCES += CommandPlotCandle.cpp
SOURCES += CommandPlotDash.cpp
SOURCES += CommandPlotDot.cpp
SOURCES += CommandPlotLine.cpp
SOURCES += CommandPlotHistogram.cpp
SOURCES += CommandPlotHistogramBar.cpp
SOURCES += CommandPlotOHLC.cpp
SOURCES += CommandROC.cpp
SOURCES += CommandRSI.cpp
SOURCES += CommandSAR.cpp
SOURCES += CommandScriptDebug.cpp
SOURCES += CommandScriptDone.cpp
SOURCES += CommandScriptStart.cpp
#SOURCES += CommandSelectDialog.cpp
SOURCES += CommandSINE.cpp
SOURCES += CommandSTDDEV.cpp
SOURCES += CommandStochFast.cpp
SOURCES += CommandStochRSI.cpp
SOURCES += CommandStochSlow.cpp
SOURCES += CommandSZ.cpp
SOURCES += CommandSymbol.cpp
SOURCES += CommandSymbolCurrent.cpp
SOURCES += CommandT3.cpp
SOURCES += CommandTHERM.cpp
SOURCES += CommandTRIX.cpp
SOURCES += CommandTypicalPrice.cpp
SOURCES += CommandULTOSC.cpp
SOURCES += CommandVAR.cpp
SOURCES += CommandVBP.cpp
SOURCES += CommandVFI.cpp
SOURCES += CommandVIDYA.cpp
SOURCES += CommandWILLR.cpp
SOURCES += CommandWidget.cpp
SOURCES += CommandWeightedClose.cpp
SOURCES += CommandYahooHistory.cpp
SOURCES += ConfigureButton.cpp
SOURCES += ConfigureDialog.cpp
SOURCES += ConfirmDialog.cpp
SOURCES += ControlPanel.cpp
SOURCES += CrossHairsButton.cpp
SOURCES += Curve.cpp
SOURCES += CurveBar.cpp
SOURCES += CurveCandle.cpp
SOURCES += CurveFactory.cpp
SOURCES += CurveHistogram.cpp
SOURCES += CurveLine.cpp
SOURCES += CurveOHLC.cpp

SOURCES += DataBase.cpp
SOURCES += DataWindowButton.cpp
SOURCES += DataWindow.cpp
SOURCES += DateRange.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateScaleDraw.cpp
SOURCES += Dialog.cpp
SOURCES += Doc.cpp
SOURCES += DockWidget.cpp

SOURCES += ExchangeDataBase.cpp

SOURCES += FileButton.cpp
SOURCES += FontButton.cpp

SOURCES += Globals.cpp
SOURCES += GridButton.cpp
SOURCES += GroupDataBase.cpp
SOURCES += GroupEditDialog.cpp
SOURCES += GroupPage.cpp

SOURCES += IconButton.cpp
SOURCES += IndicatorDialog.cpp
SOURCES += InfoPanel.cpp
SOURCES += InputType.cpp
SOURCES += IPCMessage.cpp

SOURCES += LineEdit.cpp

SOURCES += MAType.cpp
SOURCES += MessageSend.cpp

SOURCES += NewDialog.cpp

SOURCES += Operator.cpp

SOURCES += PickerMachine.cpp
SOURCES += Plot.cpp
SOURCES += PlotGroup.cpp
SOURCES += PlotMenu.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp

SOURCES += QuoteDataBase.cpp

SOURCES += RecentCharts.cpp
SOURCES += RefreshButton.cpp

SOURCES += Script.cpp
SOURCES += ScriptDataBase.cpp
SOURCES += ScriptDialog.cpp
SOURCES += ScriptInsertDialog.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptLaunchButtonDialog.cpp
SOURCES += ScriptPage.cpp
SOURCES += ScriptTimer.cpp
SOURCES += ScriptTimerDialog.cpp
SOURCES += SelectDialog.cpp
SOURCES += Setting.cpp
SOURCES += SettingBool.cpp
SOURCES += SettingColor.cpp
SOURCES += SettingDateTime.cpp
SOURCES += SettingDouble.cpp
SOURCES += SettingFont.cpp
SOURCES += SettingGroup.cpp
SOURCES += SettingInteger.cpp
SOURCES += SettingList.cpp
SOURCES += SettingString.cpp
SOURCES += SidePanel.cpp
SOURCES += SharedMemory.cpp
SOURCES += Strip.cpp
SOURCES += SymbolButton.cpp
SOURCES += SymbolDialog.cpp
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
QT += dbus
