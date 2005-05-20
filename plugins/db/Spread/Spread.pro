pluginName = Spread

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Spread.h

SOURCES += Spread.cpp

target.path = /usr/lib/qtstalker/db
INSTALLS += target
