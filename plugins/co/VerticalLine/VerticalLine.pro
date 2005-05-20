pluginName = VerticalLine

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += VerticalLine.h
HEADERS += VerticalLineObject.h

SOURCES += VerticalLine.cpp
SOURCES += VerticalLineObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
