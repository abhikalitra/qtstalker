pluginName = Bar 

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Bars.h
HEADERS += BarDialog.h

SOURCES += Bars.cpp
SOURCES += BarDialog.cpp

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
