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
HEADERS += BarsSpinner.h
HEADERS += GridAction.h
HEADERS += DocsAction.h
HEADERS += TabWidget.h


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
SOURCES += BarsSpinner.cpp
SOURCES += GridAction.cpp
SOURCES += DocsAction.cpp
SOURCES += TabWidget.cpp


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

QT += sql
