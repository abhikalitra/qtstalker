TEMPLATE = lib
 
CONFIG += thread warn_on
  
HEADERS += Plugin.h
HEADERS += QuotePlugin.h
HEADERS += IndicatorPlugin.h
HEADERS += Setting.h
HEADERS += FuturesData.h
HEADERS += PlotLine.h
HEADERS += ChartDb.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += ChartPlugin.h
HEADERS += PrefDialog.h
HEADERS += SymbolDialog.h
HEADERS += SymbolButton.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
HEADERS += FileButton.h
HEADERS += Toolbar.h
HEADERS += Bar.h
HEADERS += BarDate.h
   
SOURCES += Plugin.cpp
SOURCES += QuotePlugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += Setting.cpp
SOURCES += FuturesData.cpp
SOURCES += PlotLine.cpp
SOURCES += ChartDb.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += ChartPlugin.cpp
SOURCES += PrefDialog.cpp
SOURCES += SymbolDialog.cpp
SOURCES += SymbolButton.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
SOURCES += FileButton.cpp
SOURCES += Toolbar.cpp
SOURCES += Bar.cpp
SOURCES += BarDate.cpp
    
TARGET = qtstalker
VERSION = 0.23.0
     
INCLUDEPATH += ../db-2.7.7/build_unix

LIBS += -L../db-2.7.7/build_unix -ldb

# linux options
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle
       
target.path = /usr/lib
INSTALLS += target
