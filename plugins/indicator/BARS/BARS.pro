pluginName = BARS

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += BARS.h
HEADERS += BarDialog.h

SOURCES += BARS.cpp
SOURCES += BarDialog.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
