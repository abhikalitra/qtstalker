TEMPLATE = lib
 
CONFIG += thread warn_on
  
HEADERS += QuotePlugin.h
HEADERS += IndicatorPlugin.h
HEADERS += ChartPlugin.h
HEADERS += DbPlugin.h
HEADERS += Setting.h
HEADERS += FuturesData.h
HEADERS += PlotLine.h
HEADERS += ChartDb.h
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
HEADERS += BarDate.h
HEADERS += Config.h
HEADERS += FormulaEdit.h
HEADERS += FormulaInput.h
HEADERS += Indicator.h
   
SOURCES += QuotePlugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += ChartPlugin.cpp
SOURCES += DbPlugin.cpp
SOURCES += Setting.cpp
SOURCES += FuturesData.cpp
SOURCES += PlotLine.cpp
SOURCES += ChartDb.cpp
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
SOURCES += BarDate.cpp
SOURCES += Config.cpp
SOURCES += FormulaEdit.cpp
SOURCES += FormulaInput.cpp
SOURCES += Indicator.cpp
    
TARGET = qtstalker
VERSION = 0.27.0
     
INCLUDEPATH += ../db-2.7.7/build_unix

LIBS += -L../db-2.7.7/build_unix -ldb

# linux options
unix:linux-g++:LIBS += -ldl

target.path = /usr/lib
INSTALLS += target
