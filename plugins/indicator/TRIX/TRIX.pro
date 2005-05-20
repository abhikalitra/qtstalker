pluginName = TRIX

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += TRIX.h

SOURCES += TRIX.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
