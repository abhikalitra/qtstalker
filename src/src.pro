TEMPLATE = app

CONFIG += thread warn_on debug

QMAKE_CXXFLAGS += -rdynamic -ffast-math
#QMAKE_CXXFLAGS += -Os

HEADERS += Qtstalker.h
HEADERS += PortfolioDialog.h
HEADERS += Quote.h
HEADERS += Plot.h
HEADERS += DataWindow.h
HEADERS += Tester.h
HEADERS += Navigator.h
HEADERS += ChartPage.h
HEADERS += GroupPage.h
HEADERS += PortfolioPage.h
HEADERS += TestPage.h
HEADERS += IndicatorPage.h
HEADERS += IndicatorTab.h
HEADERS += NavigatorTab.h
HEADERS += ScannerPage.h
HEADERS += Scanner.h

SOURCES += Qtstalker.cpp
SOURCES += PortfolioDialog.cpp
SOURCES += Quote.cpp
SOURCES += Plot.cpp
SOURCES += DataWindow.cpp
SOURCES += Tester.cpp
SOURCES += Navigator.cpp
SOURCES += ChartPage.cpp
SOURCES += GroupPage.cpp
SOURCES += PortfolioPage.cpp
SOURCES += TestPage.cpp
SOURCES += IndicatorPage.cpp
SOURCES += IndicatorTab.cpp
SOURCES += NavigatorTab.cpp
SOURCES += ScannerPage.cpp
SOURCES += Scanner.cpp

TARGET = qtstalker

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/bin
INSTALLS += target

