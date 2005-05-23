pluginName = MINMAX

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MINMAX.h

SOURCES += MINMAX.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
