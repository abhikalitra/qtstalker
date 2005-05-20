pluginName = SYMBOL

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += SYMBOL.h

SOURCES += SYMBOL.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
