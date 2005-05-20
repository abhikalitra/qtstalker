pluginName = BB

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += BB.h

SOURCES += BB.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
