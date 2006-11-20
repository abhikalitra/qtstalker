pluginName = Yahoo

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Yahoo.h

SOURCES += Yahoo.cpp

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
