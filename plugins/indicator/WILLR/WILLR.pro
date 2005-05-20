pluginName = WILLR

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += WILLR.h

SOURCES += WILLR.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
