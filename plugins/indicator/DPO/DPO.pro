pluginName = DPO

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += DPO.h

SOURCES += DPO.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
