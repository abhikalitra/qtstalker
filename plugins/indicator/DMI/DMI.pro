pluginName = DMI

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += DMI.h

SOURCES += DMI.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
