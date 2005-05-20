pluginName = MAOSC

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MAOSC.h

SOURCES += MAOSC.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
