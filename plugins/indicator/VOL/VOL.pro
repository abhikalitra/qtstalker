pluginName = VOL

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += VOL.h

SOURCES += VOL.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
