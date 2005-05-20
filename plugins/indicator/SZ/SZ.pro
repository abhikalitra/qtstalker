pluginName = SZ

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += SZ.h

SOURCES += SZ.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
