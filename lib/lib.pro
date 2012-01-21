TEMPLATE = lib

CONFIG += thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

######################################################
################## HEADERS  #########################
######################################################

HEADERS += AboutDialog.h

HEADERS += BarLengthButton.h
HEADERS += BarSpaceButton.h

HEADERS += ChartLoad.h
HEADERS += ChartNew.h
HEADERS += ChartObject.h
HEADERS += ChartObjectBuy.h
HEADERS += ChartObjectDeleteAll.h
HEADERS += ChartObjectFactory.h
HEADERS += ChartObjectHLine.h
HEADERS += ChartObjectRetracement.h
HEADERS += ChartObjectSell.h
HEADERS += ChartObjectText.h
HEADERS += ChartObjectTLine.h
HEADERS += ChartObjectVLine.h
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
HEADERS += CommandBuy.h
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
HEADERS += CommandHLine.h
HEADERS += CommandHT.h
HEADERS += CommandLINEARREG.h
HEADERS += CommandLoad.h
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
HEADERS += CommandRetracement.h
HEADERS += CommandROC.h
HEADERS += CommandRSI.h
HEADERS += CommandRun.h
HEADERS += CommandSAR.h
HEADERS += CommandSave.h
HEADERS += CommandSell.h
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
HEADERS += CommandText.h
HEADERS += CommandTHERM.h
HEADERS += CommandTLine.h
HEADERS += CommandTRIX.h
HEADERS += CommandTypicalPrice.h
HEADERS += CommandULTOSC.h
HEADERS += CommandVAR.h
#HEADERS += CommandVBP.h
HEADERS += CommandVFI.h
HEADERS += CommandVIDYA.h
HEADERS += CommandVLine.h
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
HEADERS += CurveData.h
HEADERS += CurveFactory.h
HEADERS += CurveHistogram.h
HEADERS += CurveLine.h
HEADERS += CurveOHLC.h

HEADERS += Data.h
HEADERS += DataBase.h
HEADERS += DataDialog.h
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
HEADERS += GroupDelete.h
HEADERS += GroupEditDialog.h
HEADERS += GroupNew.h
HEADERS += GroupPage.h

HEADERS += IconButton.h
HEADERS += IndicatorAdd.h
HEADERS += IndicatorRemove.h
HEADERS += InfoPanel.h

HEADERS += KeyChartObject.h
HEADERS += KeyCurveBar.h
HEADERS += KeyCurveData.h
HEADERS += KeyGroupDataBase.h
HEADERS += KeyIndicatorDataBase.h
HEADERS += KeyQuoteDataBase.h
HEADERS += KeyScriptDataBase.h
HEADERS += KeySymbol.h

HEADERS += LineEdit.h

HEADERS += MA.h
HEADERS += MessageDialog.h

HEADERS += NewDialog.h

HEADERS += PickerMachine.h
HEADERS += Plot.h
HEADERS += PlotGroup.h
HEADERS += PlotInfo.h
HEADERS += PlotMenu.h
HEADERS += PlotPicker.h
HEADERS += PlotScaleDraw.h

HEADERS += QuoteDataBase.h

HEADERS += RecentCharts.h

HEADERS += Script.h
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
HEADERS += SymbolListWidget.h
HEADERS += SymbolLoad.h

HEADERS += TALibFunction.h
HEADERS += TALibInput.h
HEADERS += TALibOutput.h
HEADERS += ThreadMessage.h
HEADERS += Type.h
HEADERS += TypeBarLength.h
HEADERS += TypeCandle.h
HEADERS += TypeChartObject.h
HEADERS += TypeCurve.h
HEADERS += TypeCommand.h
HEADERS += TypeData.h
HEADERS += TypeDateRange.h
HEADERS += TypeEntity.h
HEADERS += TypeHistogram.h
HEADERS += TypeLine.h
HEADERS += TypeMA.h
HEADERS += TypeOHLC.h
HEADERS += TypeOperator.h
HEADERS += TypeThreadMessage.h
HEADERS += TypeTALibFunction.h
HEADERS += TypeVerb.h

HEADERS += WindowTitle.h

######################################################
################## SOURCES  #########################
######################################################

SOURCES += AboutDialog.cpp

SOURCES += BarLengthButton.cpp
SOURCES += BarSpaceButton.cpp

SOURCES += ChartLoad.cpp
SOURCES += ChartNew.cpp
SOURCES += ChartObject.cpp
SOURCES += ChartObjectBuy.cpp
SOURCES += ChartObjectDeleteAll.cpp
SOURCES += ChartObjectFactory.cpp
SOURCES += ChartObjectHLine.cpp
SOURCES += ChartObjectRetracement.cpp
SOURCES += ChartObjectSell.cpp
SOURCES += ChartObjectText.cpp
SOURCES += ChartObjectTLine.cpp
SOURCES += ChartObjectVLine.cpp
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
SOURCES += CommandBuy.cpp
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
SOURCES += CommandHLine.cpp
SOURCES += CommandHT.cpp
SOURCES += CommandLINEARREG.cpp
SOURCES += CommandLoad.cpp
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
SOURCES += CommandRetracement.cpp
SOURCES += CommandROC.cpp
SOURCES += CommandRSI.cpp
SOURCES += CommandRun.cpp
SOURCES += CommandSAR.cpp
SOURCES += CommandSave.cpp
SOURCES += CommandSell.cpp
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
SOURCES += CommandText.cpp
SOURCES += CommandTHERM.cpp
SOURCES += CommandTLine.cpp
SOURCES += CommandTRIX.cpp
SOURCES += CommandTypicalPrice.cpp
SOURCES += CommandULTOSC.cpp
SOURCES += CommandVAR.cpp
#SOURCES += CommandVBP.cpp
SOURCES += CommandVFI.cpp
SOURCES += CommandVIDYA.cpp
SOURCES += CommandVLine.cpp
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
SOURCES += CurveData.cpp
SOURCES += CurveFactory.cpp
SOURCES += CurveHistogram.cpp
SOURCES += CurveLine.cpp
SOURCES += CurveOHLC.cpp

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
SOURCES += GroupDelete.cpp
SOURCES += GroupEditDialog.cpp
SOURCES += GroupNew.cpp
SOURCES += GroupPage.cpp

SOURCES += IconButton.cpp
SOURCES += IndicatorAdd.cpp
SOURCES += IndicatorRemove.cpp
SOURCES += InfoPanel.cpp

SOURCES += KeyChartObject.cpp
SOURCES += KeyCurveBar.cpp
SOURCES += KeyCurveData.cpp
SOURCES += KeyGroupDataBase.cpp
SOURCES += KeyIndicatorDataBase.cpp
SOURCES += KeyQuoteDataBase.cpp
SOURCES += KeyScriptDataBase.cpp
SOURCES += KeySymbol.cpp

SOURCES += LineEdit.cpp

SOURCES += MA.cpp
SOURCES += MessageDialog.cpp

SOURCES += NewDialog.cpp

SOURCES += PickerMachine.cpp
SOURCES += Plot.cpp
SOURCES += PlotGroup.cpp
SOURCES += PlotInfo.cpp
SOURCES += PlotMenu.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp

SOURCES += QuoteDataBase.cpp

SOURCES += RecentCharts.cpp

SOURCES += Script.cpp
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
SOURCES += SymbolListWidget.cpp
SOURCES += SymbolLoad.cpp

SOURCES += TALibFunction.cpp
SOURCES += TALibInput.cpp
SOURCES += TALibOutput.cpp
SOURCES += ThreadMessage.cpp
SOURCES += Type.cpp
SOURCES += TypeBarLength.cpp
SOURCES += TypeCandle.cpp
SOURCES += TypeChartObject.cpp
SOURCES += TypeCommand.cpp
SOURCES += TypeCurve.cpp
SOURCES += TypeDateRange.cpp
SOURCES += TypeHistogram.cpp
SOURCES += TypeLine.cpp
SOURCES += TypeMA.cpp
SOURCES += TypeOHLC.cpp
SOURCES += TypeOperator.cpp
SOURCES += TypeTALibFunction.cpp
SOURCES += TypeVerb.cpp

SOURCES += WindowTitle.cpp


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
