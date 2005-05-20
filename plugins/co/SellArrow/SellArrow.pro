pluginName = SellArrow

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += SellArrow.h
HEADERS += SellArrowObject.h

SOURCES += SellArrow.cpp
SOURCES += SellArrowObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
