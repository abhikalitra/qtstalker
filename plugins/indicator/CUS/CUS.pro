pluginName = CUS

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CUS.h
HEADERS += CUSDialog.h

SOURCES += CUS.cpp
SOURCES += CUSDialog.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
