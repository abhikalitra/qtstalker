TEMPLATE = app

CONFIG += thread warn_on debug

TMAKE_CXXFLAGS += -rdynamic

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
HEADERS	+= CompositeDialog.h
HEADERS	+= WorkwithCharts.h
HEADERS	+= Plugin.h

HEADERS	+= WorkwithChartsDialog.h
HEADERS	+= WorkwithGroupsDialog.h
HEADERS	+= WorkwithPortfoliosDialog.h

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
SOURCES	+= CompositeDialog.cpp
SOURCES += WorkwithCharts.cpp
SOURCES += Plugin.cpp

SOURCES += WorkwithChartsDialog.cpp
SOURCES += WorkwithGroupsDialog.cpp
SOURCES += WorkwithPortfoliosDialog.cpp

TARGET = qtstalker

INCLUDEPATH += db-2.7.7/build_unix

LIBS += -Ldb-2.7.7/build_unix -ldb -ldl
LIBS += -L/usr/lib




