TEMPLATE = app

CONFIG += qt thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += BarLengthButtons.h
HEADERS += ChartLayout.h
HEADERS += ChartObject.h
HEADERS += ChartObjectBuy.h
#HEADERS += ChartObjectBuyDialog.h
HEADERS += ChartObjectBuyDraw.h
#HEADERS += ChartObjectDialog.h
HEADERS += ChartObjectDraw.h
HEADERS += ChartObjectFactory.h
HEADERS += ChartObjectHLine.h
#HEADERS += ChartObjectHLineDialog.h
HEADERS += ChartObjectHLineDraw.h
HEADERS += ChartObjectRetracement.h
#HEADERS += ChartObjectRetracementDialog.h
HEADERS += ChartObjectRetracementDraw.h
HEADERS += ChartObjectSell.h
#HEADERS += ChartObjectSellDialog.h
HEADERS += ChartObjectSellDraw.h
#HEADERS += ChartObjectSettings.h
HEADERS += ChartObjectText.h
#HEADERS += ChartObjectTextDialog.h
HEADERS += ChartObjectTextDraw.h
HEADERS += ChartObjectTLine.h
#HEADERS += ChartObjectTLineDialog.h
HEADERS += ChartObjectTLineDraw.h
HEADERS += ChartObjectVLine.h
#HEADERS += ChartObjectVLineDialog.h
HEADERS += ChartObjectVLineDraw.h
HEADERS += CrossHairsButton.h
HEADERS += DataWindow.h
HEADERS += DateRange.h
HEADERS += DateRangeButton.h
HEADERS += DateRangeControl.h
HEADERS += DateRangeDialog.h
HEADERS += DateScaleDraw.h
HEADERS += Dialog.h
HEADERS += DocsAction.h
HEADERS += GridAction.h
HEADERS += InfoPanel.h
HEADERS += ListWidget.h
#HEADERS += MessageServer.h
HEADERS += OHLC.h
HEADERS += PickerMachine.h
HEADERS += PixelSpaceButton.h
HEADERS += Plot.h
HEADERS += PlotCandle.h
HEADERS += PlotHistogramBar.h
HEADERS += PlotOHLC.h
HEADERS += PlotPicker.h
HEADERS += PlotSettings.h
HEADERS += PlotScaleDraw.h
HEADERS += PlotSlider.h
HEADERS += Qtstalker.h
HEADERS += RecentCharts.h
HEADERS += RefreshAction.h
HEADERS += Setup.h
HEADERS += SidePanel.h
HEADERS += SidePanelButton.h
HEADERS += Splitter.h
HEADERS += TabWidget.h
HEADERS += TabWidgetDialog.h
HEADERS += ZoomButtons.h


SOURCES += BarLengthButtons.cpp
SOURCES += ChartLayout.cpp
SOURCES += ChartObject.cpp
SOURCES += ChartObjectBuy.cpp
#SOURCES += ChartObjectBuyDialog.cpp
SOURCES += ChartObjectBuyDraw.cpp
#SOURCES += ChartObjectDialog.cpp
SOURCES += ChartObjectDraw.cpp
SOURCES += ChartObjectFactory.cpp
SOURCES += ChartObjectHLine.cpp
#SOURCES += ChartObjectHLineDialog.cpp
SOURCES += ChartObjectHLineDraw.cpp
SOURCES += ChartObjectRetracement.cpp
#SOURCES += ChartObjectRetracementDialog.cpp
SOURCES += ChartObjectRetracementDraw.cpp
SOURCES += ChartObjectSell.cpp
#SOURCES += ChartObjectSellDialog.cpp
SOURCES += ChartObjectSellDraw.cpp
SOURCES += ChartObjectText.cpp
#SOURCES += ChartObjectTextDialog.cpp
SOURCES += ChartObjectTextDraw.cpp
SOURCES += ChartObjectTLine.cpp
#SOURCES += ChartObjectTLineDialog.cpp
SOURCES += ChartObjectTLineDraw.cpp
SOURCES += ChartObjectVLine.cpp
#SOURCES += ChartObjectVLineDialog.cpp
SOURCES += ChartObjectVLineDraw.cpp
SOURCES += CrossHairsButton.cpp
SOURCES += DataWindow.cpp
SOURCES += DateRange.cpp
SOURCES += DateRangeButton.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateRangeDialog.cpp
SOURCES += DateScaleDraw.cpp
SOURCES += Dialog.cpp
SOURCES += DocsAction.cpp
SOURCES += GridAction.cpp
SOURCES += InfoPanel.cpp
SOURCES += ListWidget.cpp
SOURCES += main.cpp
#SOURCES += MessageServer.cpp
SOURCES += PickerMachine.cpp
SOURCES += PixelSpaceButton.cpp
SOURCES += Plot.cpp
SOURCES += PlotCandle.cpp
SOURCES += PlotHistogramBar.cpp
SOURCES += PlotOHLC.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp
SOURCES += PlotSlider.cpp
SOURCES += Qtstalker.cpp
SOURCES += RecentCharts.cpp
SOURCES += RefreshAction.cpp
SOURCES += Setup.cpp
SOURCES += SidePanel.cpp
SOURCES += SidePanelButton.cpp
SOURCES += Splitter.cpp
SOURCES += TabWidget.cpp
SOURCES += TabWidgetDialog.cpp
SOURCES += ZoomButtons.cpp


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

