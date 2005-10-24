pluginName = BARS

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += BARS.h

SOURCES += BARS.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
