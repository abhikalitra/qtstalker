pluginName = AD

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += AD.h

SOURCES += AD.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
