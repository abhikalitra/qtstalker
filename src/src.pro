TEMPLATE = app

CONFIG += thread warn_on debug

QMAKE_CXXFLAGS += -rdynamic
QMAKE_CXXFLAGS += -Os

HEADERS += Qtstalker.h
HEADERS += PortfolioDialog.h
HEADERS += Indicator.h
HEADERS += Quote.h
HEADERS += Config.h
HEADERS += Plot.h
HEADERS += DataWindow.h
HEADERS += EditDialog.h
HEADERS += Tester.h
HEADERS += SettingView.h
HEADERS += Navigator.h
HEADERS += EditChartDialog.h
HEADERS += SymbolDialog.h
HEADERS += BaseDialog.h
HEADERS += ChartPage.h
HEADERS += GroupPage.h
HEADERS += PortfolioPage.h
HEADERS += TestPage.h
HEADERS += IndicatorPage.h
HEADERS += NewIndicatorDialog.h
#HEADERS += ScannerPage.h
#HEADERS += Scanner.h
HEADERS += Scaler.h


SOURCES += Qtstalker.cpp
SOURCES += PortfolioDialog.cpp
SOURCES += Indicator.cpp
SOURCES += Quote.cpp
SOURCES += Config.cpp
SOURCES += Plot.cpp
SOURCES += DataWindow.cpp
SOURCES += EditDialog.cpp
SOURCES += Tester.cpp
SOURCES += SettingView.cpp
SOURCES += Navigator.cpp
SOURCES += EditChartDialog.cpp
SOURCES += SymbolDialog.cpp
SOURCES += BaseDialog.cpp
SOURCES += ChartPage.cpp
SOURCES += GroupPage.cpp
SOURCES += PortfolioPage.cpp
SOURCES += TestPage.cpp
SOURCES += IndicatorPage.cpp
SOURCES += NewIndicatorDialog.cpp
#SOURCES += ScannerPage.cpp
#SOURCES += Scanner.cpp
SOURCES += Scaler.cpp

TARGET = qtstalker

INCLUDEPATH += ../lib
INCLUDEPATH += ../db-2.7.7/build_unix

LIBS += -L../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/bin
INSTALLS += target

