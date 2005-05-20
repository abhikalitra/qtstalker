pluginName = Futures

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Futures.h
HEADERS += FuturesDialog.h

SOURCES += Futures.cpp
SOURCES += FuturesDialog.cpp

target.path = /usr/lib/qtstalker/db
INSTALLS += target
