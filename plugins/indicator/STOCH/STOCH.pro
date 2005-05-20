pluginName = STOCH

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += STOCH.h

SOURCES += STOCH.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
