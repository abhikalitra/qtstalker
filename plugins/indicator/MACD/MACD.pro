pluginName = MACD

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MACD.h

SOURCES += MACD.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
