pluginName = VOLR

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += VOLR.h

SOURCES += VOLR.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
