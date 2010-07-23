TEMPLATE = app

CONFIG += qt thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += Qtstalker.h
HEADERS += assistant.h
HEADERS += DataWindow.h
HEADERS += ChartPage.h
HEADERS += GroupPage.h
HEADERS += IndicatorPage.h
HEADERS += ScriptPage.h
HEADERS += InfoPanel.h
HEADERS += PlotSlider.h
HEADERS += COToolBar.h
HEADERS += ZoomButtons.h
HEADERS += BarLengthButtons.h
HEADERS += RefreshAction.h
HEADERS += RecentCharts.h
HEADERS += ScriptLaunchButton.h
HEADERS += ListWidget.h
HEADERS += PluginPage.h
HEADERS += PixelSpaceButton.h
HEADERS += GridAction.h
HEADERS += DocsAction.h
HEADERS += TabWidget.h
HEADERS += Splitter.h
HEADERS += DateRangeControl.h
HEADERS += DateRangeButton.h
HEADERS += SidePanel.h
HEADERS += ChartLayout.h
HEADERS += IndicatorDialog.h
HEADERS += UpdateChartPageThread.h
HEADERS += UpdateGroupPageThread.h
HEADERS += SidePanelButton.h


SOURCES += Qtstalker.cpp
SOURCES += main.cpp
SOURCES += assistant.cpp
SOURCES += DataWindow.cpp
SOURCES += ChartPage.cpp
SOURCES += GroupPage.cpp
SOURCES += IndicatorPage.cpp
SOURCES += ScriptPage.cpp
SOURCES += InfoPanel.cpp
SOURCES += PlotSlider.cpp
SOURCES += COToolBar.cpp
SOURCES += ZoomButtons.cpp
SOURCES += BarLengthButtons.cpp
SOURCES += RefreshAction.cpp
SOURCES += RecentCharts.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ListWidget.cpp
SOURCES += PluginPage.cpp
SOURCES += PixelSpaceButton.cpp
SOURCES += GridAction.cpp
SOURCES += DocsAction.cpp
SOURCES += TabWidget.cpp
SOURCES += Splitter.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DateRangeButton.cpp
SOURCES += SidePanel.cpp
SOURCES += ChartLayout.cpp
SOURCES += IndicatorDialog.cpp
SOURCES += UpdateChartPageThread.cpp
SOURCES += UpdateGroupPageThread.cpp
SOURCES += SidePanelButton.cpp



TARGET = qtstalker

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

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
