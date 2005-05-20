pluginName = FI

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += FI.h

SOURCES += FI.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
