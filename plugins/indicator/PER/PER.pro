pluginName = PER

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += PER.h

SOURCES += PER.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
