TEMPLATE = app

CONFIG += thread warn_on debug

QMAKE_CXXFLAGS += -rdynamic
#QMAKE_CXXFLAGS += -Os

HEADERS += Qtstalker.h
HEADERS += PortfolioDialog.h
HEADERS += Indicator.h
HEADERS += Quote.h
HEADERS += ChartDb.h
HEADERS += Config.h
HEADERS += Plot.h
HEADERS += Setting.h
HEADERS += DataWindow.h
HEADERS += FuturesData.h
HEADERS += EditDialog.h
HEADERS += Plugin.h
HEADERS += PlotLine.h
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

SOURCES += Qtstalker.cpp
SOURCES += PortfolioDialog.cpp
SOURCES += Indicator.cpp
SOURCES += Quote.cpp
SOURCES += ChartDb.cpp
SOURCES += Config.cpp
SOURCES += Plot.cpp
SOURCES += Setting.cpp
SOURCES += DataWindow.cpp
SOURCES += FuturesData.cpp
SOURCES += EditDialog.cpp
SOURCES += Plugin.cpp
SOURCES += PlotLine.cpp
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

TARGET = qtstalker

INCLUDEPATH += ../db-2.7.7/build_unix

LIBS += -L../db-2.7.7/build_unix -ldb
LIBS += -L/usr/lib

# linux options
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

target.path = /usr/bin
INSTALLS += target

docs.path = /usr/share/doc/qtstalker
docs.files = ../docs/*
INSTALLS += docs



