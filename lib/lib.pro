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
HEADERS += Traverse.h
HEADERS += DataWindow.h
HEADERS += IndicatorSummary.h
HEADERS += XmlWriter.h
HEADERS += DBBase.h
HEADERS += DBIndex.h
HEADERS += DBIndexItem.h
HEADERS += BARS.h
HEADERS += CUS.h
HEADERS += ExScript.h
HEADERS += FI.h
HEADERS += LMS.h
HEADERS += LOWPASS.h
HEADERS += PP.h
HEADERS += SINWAV.h
HEADERS += SYMBOL.h
HEADERS += SZ.h
HEADERS += TALIB.h
HEADERS += THERM.h
HEADERS += UTIL.h
HEADERS += VFI.h
HEADERS += VIDYA.h
HEADERS += VOL.h
HEADERS += qtsFFT.h
HEADERS += CUSDialog.h
HEADERS += Preferences.h
HEADERS += Exchange.h
HEADERS += ToolBarBtn.h
HEADERS += RcFile.h
   
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
SOURCES += Traverse.cpp
SOURCES += DataWindow.cpp
SOURCES += IndicatorSummary.cpp
SOURCES += XmlWriter.cpp
SOURCES += DBBase.cpp
SOURCES += DBIndex.cpp
SOURCES += DBIndexItem.cpp
SOURCES += BARS.cpp
SOURCES += CUS.cpp
SOURCES += ExScript.cpp
SOURCES += FI.cpp
SOURCES += LMS.cpp
SOURCES += LOWPASS.cpp
SOURCES += PP.cpp
SOURCES += SINWAV.cpp
SOURCES += SYMBOL.cpp
SOURCES += SZ.cpp
SOURCES += TALIB.cpp
SOURCES += THERM.cpp
SOURCES += UTIL.cpp
SOURCES += VFI.cpp
SOURCES += VIDYA.cpp
SOURCES += VOL.cpp
SOURCES += qtsFFT.cpp
SOURCES += CUSDialog.cpp
SOURCES += Preferences.cpp
SOURCES += Exchange.cpp
SOURCES += ToolBarBtn.cpp
SOURCES += RcFile.cpp
   
TARGET = qtstalker
VERSION = 0.37.0

LIBS += -ldb

TA_LIB_VERSION = $$system(ta-lib-config --version)
contains(TA_LIB_VERSION, 0.3.0) {
  LIBS += -lta_abstract
  LIBS += -lta_common
  LIBS += -lta_func
} else {
  LIBS += -lta_lib
}

unix:INCLUDEPATH += /usr/local/include/ta-lib
unix:LIBS += -L/usr/local/lib
     
# linux options
unix:linux-g++:LIBS += -ldl

# FreeBSD options
unix:freebsd-g++:INCLUDEPATH += /usr/X11R6/include
unix:freebsd-g++:INCLUDEPATH += /usr/local/include/db43
unix:freebsd-g++:LIBS += -L/usr/local/lib/db43

target.path = /usr/local/lib
INSTALLS += target

#The following line was inserted by qt3to4
QT +=  qt3support 
