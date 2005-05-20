pluginName = Text

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Text.h
HEADERS += TextObject.h

SOURCES += Text.cpp
SOURCES += TextObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
