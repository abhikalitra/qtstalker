pluginName = VFI

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += VFI.h

SOURCES += VFI.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
