!include( ../../../misc.plugin.config ){
  message( "Oops -- No custom build options specified" )
}


HEADERS += Yahoo.h
HEADERS += YahooDialog.h
HEADERS += YahooThread.h
HEADERS += YahooSymbolDialog.h
HEADERS += YahooDataBase.h
HEADERS += YahooConfig.h


SOURCES += Yahoo.cpp
SOURCES += YahooDialog.cpp
SOURCES += YahooThread.cpp
SOURCES += YahooSymbolDialog.cpp
SOURCES += YahooDataBase.cpp
SOURCES += YahooConfig.cpp


QT += network
QT += sql

