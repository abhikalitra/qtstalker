pluginName = REF

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEREFERS += REF.h

SOURCES += REF.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
