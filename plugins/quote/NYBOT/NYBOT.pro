pluginName = NYBOT

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += NYBOT.h

SOURCES += NYBOT.cpp

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
