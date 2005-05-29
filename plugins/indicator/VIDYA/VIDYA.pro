pluginName = VIDYA

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += VIDYA.h

SOURCES += VIDYA.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target

