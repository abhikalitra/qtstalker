TEMPLATE = app

CONFIG += qt thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += assistant.h
HEADERS += BarLengthButtons.h
HEADERS += ChartLayout.h
HEADERS += ChartPage.h
HEADERS += DataWindow.h
HEADERS += DateRangeButton.h
HEADERS += DateRangeControl.h
HEADERS += DateRangeDialog.h
HEADERS += DocsAction.h
HEADERS += GridAction.h
HEADERS += GroupAddDialog.h
HEADERS += GroupDeleteDialog.h
HEADERS += GroupDeleteItemsDialog.h
HEADERS += GroupNewDialog.h
HEADERS += GroupPage.h
HEADERS += IndicatorDialog.h
HEADERS += InfoPanel.h
HEADERS += ListWidget.h
HEADERS += PixelSpaceButton.h
HEADERS += PlotSlider.h
HEADERS += PluginPage.h
HEADERS += QuoteServerDialog.h
HEADERS += Qtstalker.h
HEADERS += RecentCharts.h
HEADERS += RefreshAction.h
HEADERS += ScriptDialog.h
HEADERS += ScriptDeleteDialog.h
HEADERS += ScriptLaunchButton.h
HEADERS += ScriptNewDialog.h
HEADERS += ScriptPage.h
HEADERS += ScriptSearchDialog.h
HEADERS += SidePanel.h
HEADERS += SidePanelButton.h
HEADERS += Splitter.h
HEADERS += SymbolListWidget.h
HEADERS += TabWidget.h
HEADERS += TabWidgetDialog.h
HEADERS += UpdateChartPageThread.h
HEADERS += UpdateGroupPageThread.h
HEADERS += ZoomButtons.h


SOURCES += assistant.cpp
SOURCES += BarLengthButtons.cpp
SOURCES += ChartLayout.cpp
SOURCES += ChartPage.cpp
SOURCES += DataWindow.cpp
SOURCES += DateRangeButton.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateRangeDialog.cpp
SOURCES += DocsAction.cpp
SOURCES += GridAction.cpp
SOURCES += GroupAddDialog.cpp
SOURCES += GroupDeleteDialog.cpp
SOURCES += GroupDeleteItemsDialog.cpp
SOURCES += GroupNewDialog.cpp
SOURCES += GroupPage.cpp
SOURCES += IndicatorDialog.cpp
SOURCES += InfoPanel.cpp
SOURCES += ListWidget.cpp
SOURCES += main.cpp
SOURCES += PixelSpaceButton.cpp
SOURCES += PlotSlider.cpp
SOURCES += PluginPage.cpp
SOURCES += QuoteServerDialog.cpp
SOURCES += Qtstalker.cpp
SOURCES += RecentCharts.cpp
SOURCES += RefreshAction.cpp
SOURCES += ScriptDialog.cpp
SOURCES += ScriptDeleteDialog.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptNewDialog.cpp
SOURCES += ScriptPage.cpp
SOURCES += ScriptSearchDialog.cpp
SOURCES += SidePanel.cpp
SOURCES += SidePanelButton.cpp
SOURCES += Splitter.cpp
SOURCES += SymbolListWidget.cpp
SOURCES += TabWidget.cpp
SOURCES += TabWidgetDialog.cpp
SOURCES += UpdateChartPageThread.cpp
SOURCES += UpdateGroupPageThread.cpp
SOURCES += ZoomButtons.cpp



TARGET = qtstalker

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker
LIBS += -lqwt

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += ../.qmake.cache

QT += core
QT += gui
QT += sql
QT += network

