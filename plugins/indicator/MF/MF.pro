pluginName = MF

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MF.h

SOURCES += MF.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
