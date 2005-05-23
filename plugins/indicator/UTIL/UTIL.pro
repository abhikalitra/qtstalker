pluginName = UTIL

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += UTIL.h

SOURCES += UTIL.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
