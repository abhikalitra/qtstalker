pluginName = UO

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += UO.h

SOURCES += UO.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
