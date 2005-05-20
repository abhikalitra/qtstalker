pluginName = CCI

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CCI.h

SOURCES += CCI.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
