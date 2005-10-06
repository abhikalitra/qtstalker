pluginName = VOLA

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += VOLA.h

SOURCES += VOLA.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
