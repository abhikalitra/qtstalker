pluginName = CME

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CME.h

SOURCES += CME.cpp

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
