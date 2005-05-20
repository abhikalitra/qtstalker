pluginName = VT

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += VT.h

SOURCES += VT.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
