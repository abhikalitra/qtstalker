pluginName = Yahoo

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Yahoo.h
HEADERS += YahooDialog.h

SOURCES += Yahoo.cpp
SOURCES += YahooDialog.cpp

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
