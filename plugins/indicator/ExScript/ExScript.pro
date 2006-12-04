pluginName = ExScript

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += ExScript.h

SOURCES += ExScript.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
