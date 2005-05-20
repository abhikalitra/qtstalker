pluginName = SD

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += SD.h

SOURCES += SD.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
