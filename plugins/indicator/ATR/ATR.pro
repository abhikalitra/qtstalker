pluginName = ATR

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += ATR.h

SOURCES += ATR.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
