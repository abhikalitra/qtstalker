pluginName = MATH

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MATH.h

SOURCES += MATH.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
