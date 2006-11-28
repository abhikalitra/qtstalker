TEMPLATE = app

CONFIG += qt thread warn_on debug

QMAKE_CXXFLAGS += -rdynamic -ffast-math
#QMAKE_CXXFLAGS += -Os

HEADERS += Qtstalker.h
HEADERS += PortfolioDialog.h
HEADERS += DataWindow.h
HEADERS += Tester.h
HEADERS += ChartPage.h
HEADERS += GroupPage.h
HEADERS += PortfolioPage.h
HEADERS += TestPage.h
HEADERS += IndicatorPage.h
HEADERS += NavigatorTab.h
HEADERS += ScannerPage.h
HEADERS += Scanner.h
HEADERS += ChartToolbar.h
HEADERS += MainMenubar.h
HEADERS += TesterReport.h
HEADERS += TesterRulePage.h
HEADERS += TesterTestPage.h
HEADERS += TesterStopPage.h
HEADERS += TesterChartPage.h


SOURCES += Qtstalker.cpp
SOURCES += PortfolioDialog.cpp
SOURCES += DataWindow.cpp
SOURCES += Tester.cpp
SOURCES += ChartPage.cpp
SOURCES += GroupPage.cpp
SOURCES += PortfolioPage.cpp
SOURCES += TestPage.cpp
SOURCES += IndicatorPage.cpp
SOURCES += NavigatorTab.cpp
SOURCES += ScannerPage.cpp
SOURCES += Scanner.cpp
SOURCES += ChartToolbar.cpp
SOURCES += MainMenubar.cpp
SOURCES += TesterReport.cpp
SOURCES += TesterRulePage.cpp
SOURCES += TesterTestPage.cpp
SOURCES += TesterStopPage.cpp
SOURCES += TesterChartPage.cpp


TARGET = qtstalker

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

# FreeBSD options
unix:freebsd-g++:INCLUDEPATH += /usr/X11R6/include

target.path = /usr/bin
INSTALLS += target

