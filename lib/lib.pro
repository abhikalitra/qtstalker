TEMPLATE = lib
 
CONFIG += thread warn_on debug
  
QMAKE_CXXFLAGS += -ffast-math
#QMAKE CXXFLAGS += -Os

HEADERS += QuotePlugin.h
HEADERS += IndicatorPlugin.h
HEADERS += DbPlugin.h
HEADERS += Setting.h
HEADERS += FuturesData.h
HEADERS += PlotLine.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += PrefDialog.h
HEADERS += SymbolDialog.h
HEADERS += SymbolButton.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
HEADERS += FileButton.h
HEADERS += Toolbar.h
HEADERS += Bar.h
HEADERS += Config.h
HEADERS += FormulaEdit.h
HEADERS += Indicator.h
HEADERS += COBase.h
HEADERS += HelpWindow.h
HEADERS += Plot.h
HEADERS += DatePlot.h
HEADERS += ScalePlot.h
HEADERS += IndicatorPlot.h
HEADERS += Navigator.h
HEADERS += BarEdit.h
HEADERS += UpgradeMessage.h
HEADERS += BuyArrow.h
HEADERS += Cycle.h
HEADERS += FiboLine.h
HEADERS += HorizontalLine.h
HEADERS += SellArrow.h
HEADERS += Text.h
HEADERS += TrendLine.h
HEADERS += VerticalLine.h
HEADERS += TradeItem.h
HEADERS += StocksDialog.h
HEADERS += FuturesDialog.h
HEADERS += IndexDialog.h
HEADERS += CC.h
HEADERS += Spread.h
HEADERS += Index.h

   
SOURCES += QuotePlugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += DbPlugin.cpp
SOURCES += Setting.cpp
SOURCES += FuturesData.cpp
SOURCES += PlotLine.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += PrefDialog.cpp
SOURCES += SymbolDialog.cpp
SOURCES += SymbolButton.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
SOURCES += FileButton.cpp
SOURCES += Toolbar.cpp
SOURCES += Bar.cpp
SOURCES += Config.cpp
SOURCES += FormulaEdit.cpp
SOURCES += Indicator.cpp
SOURCES += COBase.cpp
SOURCES += HelpWindow.cpp
SOURCES += Plot.cpp
SOURCES += DatePlot.cpp
SOURCES += ScalePlot.cpp
SOURCES += IndicatorPlot.cpp
SOURCES += Navigator.cpp
SOURCES += BarEdit.cpp
SOURCES += UpgradeMessage.cpp
SOURCES += BuyArrow.cpp
SOURCES += Cycle.cpp
SOURCES += FiboLine.cpp
SOURCES += HorizontalLine.cpp
SOURCES += SellArrow.cpp
SOURCES += Text.cpp
SOURCES += TrendLine.cpp
SOURCES += VerticalLine.cpp
SOURCES += TradeItem.cpp
SOURCES += StocksDialog.cpp
SOURCES += FuturesDialog.cpp
SOURCES += IndexDialog.cpp
SOURCES += CC.cpp
SOURCES += Spread.cpp
SOURCES += Index.cpp

    
TARGET = qtstalker
VERSION = 0.34.0

LIBS += -ldb
     
# linux options
unix:linux-g++:LIBS += -ldl

# FreeBSD options
unix:freebsd-g++:INCLUDEPATH += /usr/X11R6/include
unix:freebsd-g++:INCLUDEPATH += /usr/local/include/db43
unix:freebsd-g++:LIBS += -L/usr/local/lib/db43

target.path = /usr/lib
INSTALLS += target

# remove this
