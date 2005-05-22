pluginName = AdaptSTOCH

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += AdaptSTOCH.h

SOURCES += AdaptSTOCH.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
