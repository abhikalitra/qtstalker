pluginName = Stocks

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Stocks.h
HEADERS += StocksDialog.h

SOURCES += Stocks.cpp
SOURCES += StocksDialog.cpp

target.path = /usr/lib/qtstalker/db
INSTALLS += target
