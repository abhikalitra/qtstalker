TEMPLATE = app

CONFIG += qt thread warn_on debug qdbus
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

#headers
HEADERS += AboutDialog.h
HEADERS += AddIndicator.h

HEADERS += BarLengthButton.h
HEADERS += BarSpaceButton.h
HEADERS += BuyDialog.h

HEADERS += Chart.h
HEADERS += ChartLoad.h
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
HEADERS += ChartUpdate.h
HEADERS += CommandMessage.h
HEADERS += ConfigureButton.h
HEADERS += ConfigureDialog.h
HEADERS += ControlPanel.h
HEADERS += CrossHairsButton.h
HEADERS += Curve.h
HEADERS += CurveFactory.h
HEADERS += CurveHistogram.h
HEADERS += CurveLine.h
HEADERS += CurveOHLC.h

HEADERS += DataWindow.h
HEADERS += DataWindowButton.h
HEADERS += DateRangeControl.h
HEADERS += DateScaleDraw.h
HEADERS += DockWidget.h

HEADERS += Globals.h
HEADERS += GridButton.h
HEADERS += GroupAdd.h
HEADERS += GroupDelete.h
HEADERS += GroupNew.h
HEADERS += GroupPage.h

HEADERS += HLineDialog.h

HEADERS += IndicatorDataBase.h
HEADERS += InfoPanel.h

HEADERS += PickerMachine.h
HEADERS += Plot.h
HEADERS += PlotGroup.h
HEADERS += PlotInfo.h
HEADERS += PlotMenu.h
HEADERS += PlotPicker.h
HEADERS += PlotScaleDraw.h

HEADERS += Qtstalker.h

HEADERS += RecentCharts.h
HEADERS += RemoveIndicator.h
HEADERS += RetracementDialog.h

HEADERS += ScriptLaunchButton.h
HEADERS += ScriptLaunchButtonDialog.h
HEADERS += ScriptPage.h
HEADERS += ScriptRunDialog.h
HEADERS += ScriptTimer.h
HEADERS += ScriptTimerDialog.h
HEADERS += SellDialog.h
HEADERS += Setup.h
HEADERS += SidePanel.h
HEADERS += SymbolDelete.h

HEADERS += TextDialog.h
HEADERS += TLineDialog.h

HEADERS += VLineDialog.h

#sources

SOURCES += AboutDialog.cpp
SOURCES += AddIndicator.cpp

SOURCES += BarLengthButton.cpp
SOURCES += BarSpaceButton.cpp
SOURCES += BuyDialog.cpp

SOURCES += Chart.cpp
SOURCES += ChartLoad.cpp
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
SOURCES += ChartUpdate.cpp
SOURCES += CommandMessage.cpp
SOURCES += ConfigureButton.cpp
SOURCES += ConfigureDialog.cpp
SOURCES += ControlPanel.cpp
SOURCES += CrossHairsButton.cpp
SOURCES += Curve.cpp
SOURCES += CurveFactory.cpp
SOURCES += CurveHistogram.cpp
SOURCES += CurveLine.cpp
SOURCES += CurveOHLC.cpp

SOURCES += DataWindow.cpp
SOURCES += DataWindowButton.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateScaleDraw.cpp
SOURCES += DockWidget.cpp

SOURCES += Globals.cpp
SOURCES += GridButton.cpp
SOURCES += GroupAdd.cpp
SOURCES += GroupDelete.cpp
SOURCES += GroupNew.cpp
SOURCES += GroupPage.cpp

SOURCES += HLineDialog.cpp

SOURCES += IndicatorDataBase.cpp
SOURCES += InfoPanel.cpp

SOURCES += main.cpp

SOURCES += PickerMachine.cpp
SOURCES += Plot.cpp
SOURCES += PlotGroup.cpp
SOURCES += PlotInfo.cpp
SOURCES += PlotMenu.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp

SOURCES += Qtstalker.cpp

SOURCES += RecentCharts.cpp
SOURCES += RemoveIndicator.cpp
SOURCES += RetracementDialog.cpp

SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptLaunchButtonDialog.cpp
SOURCES += ScriptPage.cpp
SOURCES += ScriptRunDialog.cpp
SOURCES += ScriptTimer.cpp
SOURCES += ScriptTimerDialog.cpp
SOURCES += SellDialog.cpp
SOURCES += Setup.cpp
SOURCES += SidePanel.cpp
SOURCES += SymbolDelete.cpp

SOURCES += TextDialog.cpp
SOURCES += TLineDialog.cpp

SOURCES += VLineDialog.cpp

TARGET = qtstalker

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
QT += dbus
