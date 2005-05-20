pluginName = THERM

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += THERM.h

SOURCES += THERM.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
