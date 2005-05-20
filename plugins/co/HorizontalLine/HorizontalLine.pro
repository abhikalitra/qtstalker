pluginName = HorizontalLine

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += HorizontalLine.h
HEADERS += HorizontalLineObject.h

SOURCES += HorizontalLine.cpp
SOURCES += HorizontalLineObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
