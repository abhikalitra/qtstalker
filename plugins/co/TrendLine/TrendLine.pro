pluginName = TrendLine

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += TrendLine.h
HEADERS += TrendLineObject.h

SOURCES += TrendLine.cpp
SOURCES += TrendLineObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
