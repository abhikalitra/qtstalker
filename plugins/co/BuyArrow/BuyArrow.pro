pluginName = BuyArrow

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += BuyArrow.h
HEADERS += BuyArrowObject.h

SOURCES += BuyArrow.cpp
SOURCES += BuyArrowObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
