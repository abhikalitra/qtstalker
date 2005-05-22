
pluginName = CMO

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CMO.h

SOURCES += CMO.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target

