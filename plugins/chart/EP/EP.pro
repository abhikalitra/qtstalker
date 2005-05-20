pluginName = EP

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += EP.h

SOURCES += EP.cpp

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
