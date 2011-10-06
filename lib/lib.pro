TEMPLATE = lib

CONFIG += thread warn_on debug qdbus
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

# headers

HEADERS += AboutDialog.h
HEADERS += AddIndicator.h

HEADERS += BarLength.h
HEADERS += BarLengthButton.h
HEADERS += BarSpaceButton.h
HEADERS += BuyDialog.h

HEADERS += CandleType.h
HEADERS += ChartLoad.h
HEADERS += ChartObject.h
HEADERS += ChartObjectBuy.h
HEADERS += ChartObjectData.h
HEADERS += ChartObjectDataBase.h
HEADERS += ChartObjectDeleteAll.h
HEADERS += ChartObjectFactory.h
HEADERS += ChartObjectHLine.h
HEADERS += ChartObjectRetracement.h
HEADERS += ChartObjectSell.h
HEADERS += ChartObjectText.h
HEADERS += ChartObjectTLine.h
HEADERS += ChartObjectVLine.h
HEADERS += ChartObjectType.h
HEADERS += ChartPage.h
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
HEADERS += CommandColor.h
HEADERS += CommandCompare.h
HEADERS += CommandCORREL.h
HEADERS += CommandCSV.h
HEADERS += CommandDialog.h
HEADERS += CommandFactory.h
HEADERS += CommandFI.h
HEADERS += CommandGroupPanelRefresh.h
HEADERS += CommandGroupSave.h
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
HEADERS += CommandNewHighLow.h
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
HEADERS += CommandSell.h
HEADERS += CommandSetting.h
HEADERS += CommandSettingGet.h
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
HEADERS += CommandThread.h
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
HEADERS += CurveType.h

HEADERS += Data.h
HEADERS += DataBase.h
HEADERS += DataDialog.h
HEADERS += DataFactory.h
HEADERS += DataWindow.h
HEADERS += DataWindowButton.h
HEADERS += DateRange.h
HEADERS += DateRangeControl.h
HEADERS += DateScaleDraw.h
HEADERS += DataSetting.h
HEADERS += Dialog.h
HEADERS += Doc.h

HEADERS += ExchangeDataBase.h
HEADERS += ExchangeSearchDialog.h

HEADERS += FileButton.h
HEADERS += FontButton.h

HEADERS += Global.h
HEADERS += GlobalControlPanel.h
HEADERS += GlobalInfoPanel.h
HEADERS += GlobalParent.h
HEADERS += GlobalPlotGroup.h
HEADERS += GlobalSidePanel.h
HEADERS += GlobalSymbol.h
HEADERS += GridButton.h
HEADERS += GroupAdd.h
HEADERS += GroupDataBase.h
HEADERS += GroupDelete.h
HEADERS += GroupEditDialog.h
HEADERS += GroupNew.h
HEADERS += GroupPage.h

HEADERS += HistogramStyle.h
HEADERS += HLineDialog.h

HEADERS += IconButton.h
HEADERS += IndicatorDataBase.h
HEADERS += InfoPanel.h

HEADERS += LineEdit.h
HEADERS += LineStyle.h

HEADERS += MAType.h
HEADERS += Message.h
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

HEADERS += RecentCharts.h
HEADERS += RemoveIndicator.h
HEADERS += RetracementDialog.h

HEADERS += Script.h
HEADERS += ScriptDataBase.h
HEADERS += ScriptLaunchButton.h
HEADERS += ScriptLaunchButtonDialog.h
HEADERS += ScriptPage.h
HEADERS += ScriptRunDialog.h
HEADERS += ScriptTimer.h
HEADERS += ScriptTimerDialog.h
HEADERS += SelectDialog.h
HEADERS += SellDialog.h
HEADERS += Setting.h
HEADERS += SettingBool.h
HEADERS += SettingColor.h
HEADERS += SettingDateTime.h
HEADERS += SettingDouble.h
HEADERS += SettingFactory.h
HEADERS += SettingFile.h
HEADERS += SettingFont.h
HEADERS += SettingInteger.h
HEADERS += SettingList.h
HEADERS += SettingString.h
HEADERS += SidePanel.h
HEADERS += Strip.h
HEADERS += Symbol.h
HEADERS += SymbolButton.h
HEADERS += SymbolDelete.h
HEADERS += SymbolDialog.h

HEADERS += TALibInput.h
HEADERS += TALibOutput.h
HEADERS += TextDialog.h
HEADERS += TLineDialog.h

HEADERS += VerifyDataInput.h
HEADERS += VLineDialog.h

#sources

SOURCES += AboutDialog.cpp
SOURCES += AddIndicator.cpp

SOURCES += BarLength.cpp
SOURCES += BarLengthButton.cpp
SOURCES += BarSpaceButton.cpp
SOURCES += BuyDialog.cpp

SOURCES += CandleType.cpp
SOURCES += ChartLoad.cpp
SOURCES += ChartObject.cpp
SOURCES += ChartObjectBuy.cpp
SOURCES += ChartObjectData.cpp
SOURCES += ChartObjectDataBase.cpp
SOURCES += ChartObjectDeleteAll.cpp
SOURCES += ChartObjectFactory.cpp
SOURCES += ChartObjectHLine.cpp
SOURCES += ChartObjectRetracement.cpp
SOURCES += ChartObjectSell.cpp
SOURCES += ChartObjectText.cpp
SOURCES += ChartObjectTLine.cpp
SOURCES += ChartObjectVLine.cpp
SOURCES += ChartObjectType.cpp
SOURCES += ChartPage.cpp
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
SOURCES += CommandColor.cpp
SOURCES += CommandCompare.cpp
SOURCES += CommandCORREL.cpp
SOURCES += CommandCSV.cpp
SOURCES += CommandDialog.cpp
SOURCES += CommandFactory.cpp
SOURCES += CommandFI.cpp
SOURCES += CommandGroupPanelRefresh.cpp
SOURCES += CommandGroupSave.cpp
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
SOURCES += CommandNewHighLow.cpp
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
SOURCES += CommandSell.cpp
SOURCES += CommandSetting.cpp
SOURCES += CommandSettingGet.cpp
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
SOURCES += CommandThread.cpp
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
SOURCES += CurveType.cpp

SOURCES += Data.cpp
SOURCES += DataBase.cpp
SOURCES += DataDialog.cpp
SOURCES += DataFactory.cpp
SOURCES += DataWindow.cpp
SOURCES += DataWindowButton.cpp
SOURCES += DateRange.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateScaleDraw.cpp
SOURCES += DataSetting.cpp
SOURCES += Dialog.cpp
SOURCES += Doc.cpp

SOURCES += ExchangeDataBase.cpp
SOURCES += ExchangeSearchDialog.cpp

SOURCES += FileButton.cpp
SOURCES += FontButton.cpp

SOURCES += Global.cpp
SOURCES += GlobalControlPanel.cpp
SOURCES += GlobalInfoPanel.cpp
SOURCES += GlobalParent.cpp
SOURCES += GlobalPlotGroup.cpp
SOURCES += GlobalSidePanel.cpp
SOURCES += GlobalSymbol.cpp
SOURCES += GridButton.cpp
SOURCES += GroupAdd.cpp
SOURCES += GroupDataBase.cpp
SOURCES += GroupDelete.cpp
SOURCES += GroupEditDialog.cpp
SOURCES += GroupNew.cpp
SOURCES += GroupPage.cpp

SOURCES += HistogramStyle.cpp
SOURCES += HLineDialog.cpp

SOURCES += IconButton.cpp
SOURCES += IndicatorDataBase.cpp
SOURCES += InfoPanel.cpp

SOURCES += LineEdit.cpp
SOURCES += LineStyle.cpp

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

SOURCES += RecentCharts.cpp
SOURCES += RemoveIndicator.cpp
SOURCES += RetracementDialog.cpp

SOURCES += Script.cpp
SOURCES += ScriptDataBase.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptLaunchButtonDialog.cpp
SOURCES += ScriptPage.cpp
SOURCES += ScriptRunDialog.cpp
SOURCES += ScriptTimer.cpp
SOURCES += ScriptTimerDialog.cpp
SOURCES += SelectDialog.cpp
SOURCES += SellDialog.cpp
SOURCES += Setting.cpp
SOURCES += SettingBool.cpp
SOURCES += SettingColor.cpp
SOURCES += SettingDateTime.cpp
SOURCES += SettingDouble.cpp
SOURCES += SettingFactory.cpp
SOURCES += SettingFile.cpp
SOURCES += SettingFont.cpp
SOURCES += SettingInteger.cpp
SOURCES += SettingList.cpp
SOURCES += SettingString.cpp
SOURCES += SidePanel.cpp
SOURCES += Strip.cpp
SOURCES += Symbol.cpp
SOURCES += SymbolButton.cpp
SOURCES += SymbolDelete.cpp
SOURCES += SymbolDialog.cpp

SOURCES += TALibInput.cpp
SOURCES += TALibOutput.cpp
SOURCES += TextDialog.cpp
SOURCES += TLineDialog.cpp

SOURCES += VerifyDataInput.cpp
SOURCES += VLineDialog.cpp

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
