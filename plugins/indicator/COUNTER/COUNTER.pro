pluginName = COUNTER

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += COUNTER.h

SOURCES += COUNTER.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
