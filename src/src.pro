TEMPLATE = app

CONFIG += qt thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += AboutDialog.h
HEADERS += BarLengthButton.h
HEADERS += BarSpaceButton.h
HEADERS += ChartPage.h
HEADERS += ConfigureButton.h
HEADERS += ConfigureDialog.h
HEADERS += ControlPanel.h
HEADERS += CrossHairsButton.h
HEADERS += DataWindowButton.h
HEADERS += DateRangeControl.h
HEADERS += DockWidget.h
HEADERS += GridButton.h
HEADERS += GroupPage.h
HEADERS += InfoPanel.h
HEADERS += NewIndicatorDialog.h
HEADERS += PickerMachine.h
HEADERS += Plot.h
HEADERS += PlotCandle.h
HEADERS += PlotHistogramBar.h
HEADERS += PlotMenu.h
HEADERS += PlotOHLC.h
HEADERS += PlotPicker.h
HEADERS += PlotScaleDraw.h
HEADERS += Qtstalker.h
HEADERS += RecentCharts.h
HEADERS += RefreshButton.h
HEADERS += ScriptLaunchButton.h
HEADERS += ScriptLaunchButtonDialog.h
HEADERS += ScriptPage.h
HEADERS += Setup.h
HEADERS += SidePanel.h
HEADERS += SymbolListWidget.h


SOURCES += AboutDialog.cpp
SOURCES += BarLengthButton.cpp
SOURCES += BarSpaceButton.cpp
SOURCES += ChartPage.cpp
SOURCES += ConfigureButton.cpp
SOURCES += ConfigureDialog.cpp
SOURCES += ControlPanel.cpp
SOURCES += CrossHairsButton.cpp
SOURCES += DataWindowButton.cpp
SOURCES += DateRangeControl.cpp
SOURCES += DockWidget.cpp
SOURCES += GridButton.cpp
SOURCES += GroupPage.cpp
SOURCES += InfoPanel.cpp
SOURCES += main.cpp
SOURCES += NewIndicatorDialog.cpp
SOURCES += PickerMachine.cpp
SOURCES += Plot.cpp
SOURCES += PlotCandle.cpp
SOURCES += PlotHistogramBar.cpp
SOURCES += PlotMenu.cpp
SOURCES += PlotOHLC.cpp
SOURCES += PlotPicker.cpp
SOURCES += PlotScaleDraw.cpp
SOURCES += Qtstalker.cpp
SOURCES += RecentCharts.cpp
SOURCES += RefreshButton.cpp
SOURCES += ScriptLaunchButton.cpp
SOURCES += ScriptLaunchButtonDialog.cpp
SOURCES += ScriptPage.cpp
SOURCES += Setup.cpp
SOURCES += SidePanel.cpp
SOURCES += SymbolListWidget.cpp


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
