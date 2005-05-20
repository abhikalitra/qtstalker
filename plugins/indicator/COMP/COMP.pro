pluginName = COMP

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += COMP.h

SOURCES += COMP.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
