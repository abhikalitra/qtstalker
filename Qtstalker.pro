TEMPLATE = app

CONFIG += thread warn_on debug

QMAKE_CXXFLAGS += -rdynamic
QMAKE_CXXFLAGS += -O2

HEADERS	+= Qtstalker.h
HEADERS	+= PortfolioDialog.h
HEADERS	+= Indicator.h
HEADERS	+= Quote.h
HEADERS	+= ChartDb.h
HEADERS	+= Config.h
HEADERS	+= Plot.h
HEADERS	+= Setting.h
HEADERS	+= GroupDialog.h
HEADERS	+= DataWindow.h
HEADERS	+= FuturesData.h
HEADERS	+= EditDialog.h
HEADERS	+= Plugin.h
HEADERS	+= WorkwithChartsDialog.h
HEADERS	+= WorkwithGroupsDialog.h
HEADERS	+= WorkwithPortfoliosDialog.h
HEADERS	+= PlotLine.h
HEADERS	+= Tester.h
HEADERS	+= SettingView.h
HEADERS	+= Navigator.h
HEADERS	+= WorkwithTestDialog.h

SOURCES += Qtstalker.cpp
SOURCES	+= PortfolioDialog.cpp
SOURCES	+= Indicator.cpp
SOURCES	+= Quote.cpp
SOURCES	+= ChartDb.cpp
SOURCES	+= Config.cpp
SOURCES	+= Plot.cpp
SOURCES	+= Setting.cpp
SOURCES	+= GroupDialog.cpp
SOURCES	+= DataWindow.cpp
SOURCES	+= FuturesData.cpp
SOURCES	+= EditDialog.cpp
SOURCES += Plugin.cpp
SOURCES += WorkwithChartsDialog.cpp
SOURCES += WorkwithGroupsDialog.cpp
SOURCES += WorkwithPortfoliosDialog.cpp
SOURCES += PlotLine.cpp
SOURCES += Tester.cpp
SOURCES += SettingView.cpp
SOURCES += Navigator.cpp
SOURCES += WorkwithTestDialog.cpp

TARGET = qtstalker

INCLUDEPATH += db-2.7.7/build_unix

LIBS += -Ldb-2.7.7/build_unix -ldb -ldl
LIBS += -L/usr/lib




