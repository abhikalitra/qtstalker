TEMPLATE = app

CONFIG += qt thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += AboutButton.h
HEADERS += AboutDialog.h
HEADERS += BarLengthButtons.h
HEADERS += ChartObject.h
HEADERS += ChartObjectBuy.h
HEADERS += ChartObjectBuyDraw.h
HEADERS += ChartObjectDraw.h
HEADERS += ChartObjectFactory.h
HEADERS += ChartObjectHLine.h
HEADERS += ChartObjectHLineDraw.h
HEADERS += ChartObjectRetracement.h
HEADERS += ChartObjectRetracementDraw.h
HEADERS += ChartObjectSell.h
HEADERS += ChartObjectSellDraw.h
HEADERS += ChartObjectText.h
HEADERS += ChartObjectTextDraw.h
HEADERS += ChartObjectTLine.h
HEADERS += ChartObjectTLineDraw.h
HEADERS += ChartObjectVLine.h
HEADERS += ChartObjectVLineDraw.h
HEADERS += ChartPage.h
HEADERS += ConfigureButton.h
HEADERS += CrossHairsButton.h
HEADERS += DataWindowButton.h
HEADERS += DateRangeButton.h
HEADERS += DateRangeControl.h
HEADERS += DateRangeDialog.h
HEADERS += DateScaleDraw.h
HEADERS += DockWidget.h
HEADERS += GridButton.h
HEADERS += GroupPage.h
HEADERS += HelpButton.h
HEADERS += InfoPanel.h
HEADERS += NewIndicatorButton.h
HEADERS += OHLC.h
HEADERS += PixelSpaceButton.h
HEADERS += PickerMachine.h
HEADERS += Plot.h
HEADERS += PlotCandle.h
HEADERS += PlotHistogramBar.h
HEADERS += PlotMenu.h
HEADERS += PlotOHLC.h
HEADERS += PlotPicker.h
HEADERS += PlotScaleDraw.h
HEADERS += PlotSlider.h
HEADERS += Qtstalker.h
HEADERS += QuitButton.h
HEADERS += RecentCharts.h
HEADERS += RefreshButton.h
HEADERS += ScriptLaunchButton.h
HEADERS += ScriptPage.h
HEADERS += Setup.h
HEADERS += SidePanel.h
HEADERS += SymbolListWidget.h
HEADERS += ZoomButtons.h


SOURCES += AboutButton.cpp
SOURCES += AboutDialog.cpp
SOURCES += BarLengthButtons.cpp
SOURCES += ChartObject.cpp
SOURCES += ChartObjectBuy.cpp
SOURCES += ChartObjectBuyDraw.cpp
SOURCES += ChartObjectDraw.cpp
SOURCES += ChartObjectFactory.cpp
SOURCES += ChartObjectHLine.cpp
SOURCES += ChartObjectHLineDraw.cpp
SOURCES += ChartObjectRetracement.cpp
SOURCES += ChartObjectRetracementDraw.cpp
SOURCES += ChartObjectSell.cpp
SOURCES += ChartObjectSellDraw.cpp
SOURCES += ChartObjectText.cpp
SOURCES += ChartObjectTextDraw.cpp
SOURCES += ChartObjectTLine.cpp
SOURCES += ChartObjectTLineDraw.cpp
SOURCES += ChartObjectVLine.cpp
SOURCES += ChartObjectVLineDraw.cpp
SOURCES += ChartPage.cpp
SOURCES += ConfigureButton.cpp
SOURCES += CrossHairsButton.cpp
SOURCES += DataWindowButton.cpp
SOURCES += DateRangeButton.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateRangeDialog.cpp
SOURCES += DateScaleDraw.cpp
SOURCES += DockWidget.cpp
SOURCES += GridButton.cpp
SOURCES += GroupPage.cpp
SOURCES += HelpButton.cpp
SOURCES += InfoPanel.cpp
SOURCES += main.cpp
SOURCES += NewIndicatorButton.cpp
SOURCES += PixelSpaceButton.cpp
SOURCES += PickerMachine.cpp
SOURCES += Plot.cpp
SOURCES += PlotCandle.cpp
SOURCES += PlotHistogramBar.cpp
SOURCES += PlotMenu.cpp
SOURCES += PlotOHLC.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp
SOURCES += PlotSlider.cpp
SOURCES += Qtstalker.cpp
SOURCES += QuitButton.cpp
SOURCES += RecentCharts.cpp
SOURCES += RefreshButton.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptPage.cpp
SOURCES += Setup.cpp
SOURCES += SidePanel.cpp
SOURCES += SymbolListWidget.cpp
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

