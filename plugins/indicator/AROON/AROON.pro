pluginName = AROON

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += AROON.h

SOURCES += AROON.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
