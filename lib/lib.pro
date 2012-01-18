TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

# headers

HEADERS += AboutDialog.h
HEADERS += AddIndicator.h

HEADERS += BarLength.h
HEADERS += BarLengthButton.h
HEADERS += BarSpaceButton.h

HEADERS += CandleType.h
HEADERS += ChartLoad.h
HEADERS += ChartNew.h
HEADERS += ChartObject.h
HEADERS += ChartObjectBuy.h
HEADERS += ChartObjectDeleteAll.h
HEADERS += ChartObjectFactory.h
HEADERS += ChartObjectHLine.h
HEADERS += ChartObjectKey.h
HEADERS += ChartObjectRetracement.h
HEADERS += ChartObjectSell.h
HEADERS += ChartObjectText.h
HEADERS += ChartObjectTLine.h
HEADERS += ChartObjectVLine.h
HEADERS += ChartObjectType.h
HEADERS += ChartPage.h
HEADERS += ChartRefresh.h
HEADERS += ChartUpdate.h
HEADERS += ColorButton.h
HEADERS += Command.h
HEADERS += CommandAD.h
HEADERS += CommandADX.h
HEADERS += CommandArithmetic.h
HEADERS += CommandAROON.h
HEADERS += CommandATR.h
HEADERS += CommandAveragePrice.h
HEADERS += CommandBBANDS.h
HEADERS += CommandBETA.h
HEADERS += CommandBOP.h
HEADERS += CommandBreakout.h
#HEADERS += CommandBuy.h
HEADERS += CommandCandlePattern.h
HEADERS += CommandCCI.h
HEADERS += CommandChart.h
HEADERS += CommandChartPanelRefresh.h
HEADERS += CommandChartUpdate.h
HEADERS += CommandCMO.h
HEADERS += CommandCompare.h
HEADERS += CommandCORREL.h
HEADERS += CommandCSV.h
HEADERS += CommandDialog.h
HEADERS += CommandFactory.h
HEADERS += CommandFI.h
HEADERS += CommandGet.h
HEADERS += CommandGroupPanelRefresh.h
HEADERS += CommandGroupSave.h
#HEADERS += CommandHLine.h
HEADERS += CommandHT.h
HEADERS += CommandLINEARREG.h
HEADERS += CommandMA.h
HEADERS += CommandMACD.h
HEADERS += CommandMAMA.h
HEADERS += CommandMedianPrice.h
HEADERS += CommandMINMAX.h
HEADERS += CommandMFI.h
HEADERS += CommandMOM.h
HEADERS += CommandNew.h
HEADERS += CommandNewHighLow.h
HEADERS += CommandNormalize.h
HEADERS += CommandParse.h
HEADERS += CommandPHASOR.h
HEADERS += CommandPO.h
HEADERS += CommandPlotLine.h
HEADERS += CommandPlotHistogram.h
HEADERS += CommandPlotOHLC.h
#HEADERS += CommandRetracement.h
HEADERS += CommandROC.h
HEADERS += CommandRSI.h
HEADERS += CommandRun.h
HEADERS += CommandSAR.h
#HEADERS += CommandSell.h
HEADERS += CommandSet.h
HEADERS += CommandShift.h
HEADERS += CommandSINE.h
HEADERS += CommandSTDDEV.h
HEADERS += CommandStochFast.h
HEADERS += CommandStochRSI.h
HEADERS += CommandStochSlow.h
HEADERS += CommandSZ.h
HEADERS += CommandSymbol.h
HEADERS += CommandSymbolCurrent.h
HEADERS += CommandT3.h
#HEADERS += CommandText.h
HEADERS += CommandTHERM.h
#HEADERS += CommandTLine.h
HEADERS += CommandTRIX.h
HEADERS += CommandTypicalPrice.h
HEADERS += CommandType.h
HEADERS += CommandULTOSC.h
HEADERS += CommandVAR.h
#HEADERS += CommandVBP.h
HEADERS += CommandVFI.h
HEADERS += CommandVIDYA.h
#HEADERS += CommandVLine.h
HEADERS += CommandWILLR.h
HEADERS += CommandWeightedClose.h
HEADERS += CommandYahooHistory.h
HEADERS += ConfigureButton.h
HEADERS += ConfigureDialog.h
HEADERS += ConfirmDialog.h
HEADERS += ControlPanel.h
HEADERS += CrossHairsButton.h
HEADERS += Curve.h
HEADERS += CurveBar.h
HEADERS += CurveBarKey.h
HEADERS += CurveData.h
HEADERS += CurveDataKey.h
HEADERS += CurveFactory.h
HEADERS += CurveHistogram.h
HEADERS += CurveLine.h
HEADERS += CurveOHLC.h
HEADERS += CurveType.h

HEADERS += Data.h
HEADERS += DataBase.h
HEADERS += DataDialog.h
HEADERS += DataType.h
HEADERS += DataWindow.h
HEADERS += DataWindowButton.h
HEADERS += DateRange.h
HEADERS += DateRangeControl.h
HEADERS += DateScaleDraw.h
HEADERS += Dialog.h
HEADERS += Doc.h

HEADERS += EAVDataBase.h
HEADERS += EAVSearch.h
HEADERS += Entity.h
HEADERS += EntityType.h

HEADERS += FileButton.h
HEADERS += FontButton.h

HEADERS += Global.h
HEADERS += GlobalControlPanel.h
HEADERS += GlobalData.h
HEADERS += GlobalInfoPanel.h
HEADERS += GlobalMutex.h
HEADERS += GlobalParent.h
HEADERS += GlobalPlotGroup.h
HEADERS += GlobalSidePanel.h
HEADERS += GlobalSymbol.h
HEADERS += GridButton.h
HEADERS += GroupAdd.h
HEADERS += GroupDataBaseKey.h
HEADERS += GroupDelete.h
HEADERS += GroupEditDialog.h
HEADERS += GroupNew.h
HEADERS += GroupPage.h

HEADERS += HistogramStyle.h

HEADERS += IconButton.h
HEADERS += IndicatorDataBaseKey.h
HEADERS += InfoPanel.h

HEADERS += Keys.h

HEADERS += LineEdit.h
HEADERS += LineStyle.h

HEADERS += MA.h
HEADERS += MAType.h
HEADERS += MessageDialog.h

HEADERS += NewDialog.h

HEADERS += OHLCStyle.h
HEADERS += Operator.h

HEADERS += PickerMachine.h
HEADERS += Plot.h
HEADERS += PlotGroup.h
HEADERS += PlotInfo.h
HEADERS += PlotMenu.h
HEADERS += PlotPicker.h
HEADERS += PlotScaleDraw.h

HEADERS += QuoteDataBase.h
HEADERS += QuoteDataBaseKey.h

HEADERS += RecentCharts.h
HEADERS += RemoveIndicator.h

HEADERS += Script.h
HEADERS += ScriptDataBaseKey.h
HEADERS += ScriptLaunchButton.h
HEADERS += ScriptLaunchButtonDialog.h
HEADERS += ScriptPage.h
HEADERS += ScriptRunDialog.h
HEADERS += ScriptTimer.h
HEADERS += ScriptTimerDialog.h
HEADERS += ScriptVerifyCurve.h
HEADERS += ScriptVerifyCurveKeys.h
HEADERS += ScriptVerifyCurveValue.h
HEADERS += SelectDialog.h
HEADERS += SidePanel.h
HEADERS += Strip.h
HEADERS += Symbol.h
HEADERS += SymbolButton.h
HEADERS += SymbolDelete.h
HEADERS += SymbolDialog.h
HEADERS += SymbolKey.h
HEADERS += SymbolLoad.h

HEADERS += TALibFunction.h
HEADERS += TALibFunctionKey.h
HEADERS += TALibInput.h
HEADERS += TALibOutput.h
HEADERS += ThreadMessage.h
HEADERS += ThreadMessageType.h


#sources

SOURCES += AboutDialog.cpp
SOURCES += AddIndicator.cpp

SOURCES += BarLength.cpp
SOURCES += BarLengthButton.cpp
SOURCES += BarSpaceButton.cpp

SOURCES += CandleType.cpp
SOURCES += ChartLoad.cpp
SOURCES += ChartNew.cpp
SOURCES += ChartObject.cpp
SOURCES += ChartObjectBuy.cpp
SOURCES += ChartObjectDeleteAll.cpp
SOURCES += ChartObjectFactory.cpp
SOURCES += ChartObjectHLine.cpp
SOURCES += ChartObjectKey.cpp
SOURCES += ChartObjectRetracement.cpp
SOURCES += ChartObjectSell.cpp
SOURCES += ChartObjectText.cpp
SOURCES += ChartObjectTLine.cpp
SOURCES += ChartObjectVLine.cpp
SOURCES += ChartObjectType.cpp
SOURCES += ChartPage.cpp
SOURCES += ChartRefresh.cpp
SOURCES += ChartUpdate.cpp
SOURCES += ColorButton.cpp
SOURCES += Command.cpp
SOURCES += CommandAD.cpp
SOURCES += CommandADX.cpp
SOURCES += CommandArithmetic.cpp
SOURCES += CommandAROON.cpp
SOURCES += CommandATR.cpp
SOURCES += CommandAveragePrice.cpp
SOURCES += CommandBBANDS.cpp
SOURCES += CommandBETA.cpp
SOURCES += CommandBOP.cpp
SOURCES += CommandBreakout.cpp
#SOURCES += CommandBuy.cpp
SOURCES += CommandCandlePattern.cpp
SOURCES += CommandCCI.cpp
SOURCES += CommandChart.cpp
SOURCES += CommandChartPanelRefresh.cpp
SOURCES += CommandChartUpdate.cpp
SOURCES += CommandCMO.cpp
SOURCES += CommandCompare.cpp
SOURCES += CommandCORREL.cpp
SOURCES += CommandCSV.cpp
SOURCES += CommandDialog.cpp
SOURCES += CommandFactory.cpp
SOURCES += CommandFI.cpp
SOURCES += CommandGet.cpp
SOURCES += CommandGroupPanelRefresh.cpp
SOURCES += CommandGroupSave.cpp
#SOURCES += CommandHLine.cpp
SOURCES += CommandHT.cpp
SOURCES += CommandLINEARREG.cpp
SOURCES += CommandMA.cpp
SOURCES += CommandMACD.cpp
SOURCES += CommandMAMA.cpp
SOURCES += CommandMedianPrice.cpp
SOURCES += CommandMINMAX.cpp
SOURCES += CommandMFI.cpp
SOURCES += CommandMOM.cpp
SOURCES += CommandNew.cpp
SOURCES += CommandNewHighLow.cpp
SOURCES += CommandNormalize.cpp
SOURCES += CommandParse.cpp
SOURCES += CommandPHASOR.cpp
SOURCES += CommandPlotLine.cpp
SOURCES += CommandPlotHistogram.cpp
SOURCES += CommandPlotOHLC.cpp
SOURCES += CommandPO.cpp
#SOURCES += CommandRetracement.cpp
SOURCES += CommandROC.cpp
SOURCES += CommandRSI.cpp
SOURCES += CommandRun.cpp
SOURCES += CommandSAR.cpp
#SOURCES += CommandSell.cpp
SOURCES += CommandSet.cpp
SOURCES += CommandShift.cpp
SOURCES += CommandSINE.cpp
SOURCES += CommandSTDDEV.cpp
SOURCES += CommandStochFast.cpp
SOURCES += CommandStochRSI.cpp
SOURCES += CommandStochSlow.cpp
SOURCES += CommandSZ.cpp
SOURCES += CommandSymbol.cpp
SOURCES += CommandSymbolCurrent.cpp
SOURCES += CommandT3.cpp
#SOURCES += CommandText.cpp
SOURCES += CommandTHERM.cpp
#SOURCES += CommandTLine.cpp
SOURCES += CommandTRIX.cpp
SOURCES += CommandTypicalPrice.cpp
SOURCES += CommandType.cpp
SOURCES += CommandULTOSC.cpp
SOURCES += CommandVAR.cpp
#SOURCES += CommandVBP.cpp
SOURCES += CommandVFI.cpp
SOURCES += CommandVIDYA.cpp
#SOURCES += CommandVLine.cpp
SOURCES += CommandWILLR.cpp
SOURCES += CommandWeightedClose.cpp
SOURCES += CommandYahooHistory.cpp
SOURCES += ConfigureButton.cpp
SOURCES += ConfigureDialog.cpp
SOURCES += ConfirmDialog.cpp
SOURCES += ControlPanel.cpp
SOURCES += CrossHairsButton.cpp
SOURCES += Curve.cpp
SOURCES += CurveBar.cpp
SOURCES += CurveBarKey.cpp
SOURCES += CurveData.cpp
SOURCES += CurveDataKey.cpp
SOURCES += CurveFactory.cpp
SOURCES += CurveHistogram.cpp
SOURCES += CurveLine.cpp
SOURCES += CurveOHLC.cpp
SOURCES += CurveType.cpp

SOURCES += Data.cpp
SOURCES += DataBase.cpp
SOURCES += DataDialog.cpp
SOURCES += DataWindow.cpp
SOURCES += DataWindowButton.cpp
SOURCES += DateRange.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateScaleDraw.cpp
SOURCES += Dialog.cpp
SOURCES += Doc.cpp

SOURCES += EAVDataBase.cpp
SOURCES += EAVSearch.cpp
SOURCES += Entity.cpp

SOURCES += FileButton.cpp
SOURCES += FontButton.cpp

SOURCES += Global.cpp
SOURCES += GlobalControlPanel.cpp
SOURCES += GlobalData.cpp
SOURCES += GlobalInfoPanel.cpp
SOURCES += GlobalMutex.cpp
SOURCES += GlobalParent.cpp
SOURCES += GlobalPlotGroup.cpp
SOURCES += GlobalSidePanel.cpp
SOURCES += GlobalSymbol.cpp
SOURCES += GridButton.cpp
SOURCES += GroupAdd.cpp
SOURCES += GroupDataBaseKey.cpp
SOURCES += GroupDelete.cpp
SOURCES += GroupEditDialog.cpp
SOURCES += GroupNew.cpp
SOURCES += GroupPage.cpp

SOURCES += HistogramStyle.cpp

SOURCES += IconButton.cpp
SOURCES += IndicatorDataBaseKey.cpp
SOURCES += InfoPanel.cpp

SOURCES += Keys.cpp

SOURCES += LineEdit.cpp
SOURCES += LineStyle.cpp

SOURCES += MA.cpp
SOURCES += MAType.cpp
SOURCES += MessageDialog.cpp

SOURCES += NewDialog.cpp

SOURCES += OHLCStyle.cpp
SOURCES += Operator.cpp

SOURCES += PickerMachine.cpp
SOURCES += Plot.cpp
SOURCES += PlotGroup.cpp
SOURCES += PlotInfo.cpp
SOURCES += PlotMenu.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp

SOURCES += QuoteDataBase.cpp
SOURCES += QuoteDataBaseKey.cpp

SOURCES += RecentCharts.cpp
SOURCES += RemoveIndicator.cpp

SOURCES += Script.cpp
SOURCES += ScriptDataBaseKey.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptLaunchButtonDialog.cpp
SOURCES += ScriptPage.cpp
SOURCES += ScriptRunDialog.cpp
SOURCES += ScriptTimer.cpp
SOURCES += ScriptTimerDialog.cpp
SOURCES += ScriptVerifyCurve.cpp
SOURCES += ScriptVerifyCurveKeys.cpp
SOURCES += ScriptVerifyCurveValue.cpp
SOURCES += SelectDialog.cpp
SOURCES += SidePanel.cpp
SOURCES += Strip.cpp
SOURCES += Symbol.cpp
SOURCES += SymbolButton.cpp
SOURCES += SymbolDelete.cpp
SOURCES += SymbolDialog.cpp
SOURCES += SymbolKey.cpp
SOURCES += SymbolLoad.cpp

SOURCES += TALibFunction.cpp
SOURCES += TALibFunctionKey.cpp
SOURCES += TALibInput.cpp
SOURCES += TALibOutput.cpp
SOURCES += ThreadMessage.cpp


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
