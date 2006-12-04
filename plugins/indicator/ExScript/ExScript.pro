pluginName = ExScript

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += ExScript.h
HEADERS += ExScriptThread.h

SOURCES += ExScript.cpp
SOURCES += ExScriptThread.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
