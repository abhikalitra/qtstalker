pluginName = RSI

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += RSI.h

SOURCES += RSI.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
