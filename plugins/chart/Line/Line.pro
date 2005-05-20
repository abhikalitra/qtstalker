pluginName = Line

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Line.h
HEADERS += LineDialog.h

SOURCES += Line.cpp
SOURCES += LineDialog.cpp

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
