pluginName = FiboLine

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += FiboLine.h
HEADERS += FiboLineObject.h

SOURCES += FiboLine.cpp
SOURCES += FiboLineObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
