pluginName = Candle 

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Candle.h

SOURCES += Candle.cpp

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
