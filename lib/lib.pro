TEMPLATE = lib
 
CONFIG += thread warn_on
  
HEADERS += Plugin.h
HEADERS += QuotePlugin.h
HEADERS += IndicatorPlugin.h
HEADERS += Setting.h
HEADERS += FuturesData.h
HEADERS += PlotLine.h
HEADERS += ChartDb.h
   
SOURCES += Plugin.cpp
SOURCES += QuotePlugin.cpp
SOURCES += IndicatorPlugin.cpp
SOURCES += Setting.cpp
SOURCES += FuturesData.cpp
SOURCES += PlotLine.cpp
SOURCES += ChartDb.cpp
    
TARGET = qtstalker
VERSION = 0.23.0
     
INCLUDEPATH += ../db-2.7.7/build_unix

LIBS += -L../db-2.7.7/build_unix -ldb
#LIBS += -L/usr/lib

# linux options
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle
       
target.path = /usr/lib
INSTALLS += target
