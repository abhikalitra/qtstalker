pluginName = CC

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CC.h

SOURCES += CC.cpp

target.path = /usr/lib/qtstalker/db
INSTALLS += target
