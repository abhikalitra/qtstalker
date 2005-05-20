pluginName = PP

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += PP.h

SOURCES += PP.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
