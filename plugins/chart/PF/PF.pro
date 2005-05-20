pluginName = PF

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += PF.h

SOURCES += PF.cpp

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
