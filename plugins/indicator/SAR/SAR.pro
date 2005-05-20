pluginName = SAR

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += SAR.h

SOURCES += SAR.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
