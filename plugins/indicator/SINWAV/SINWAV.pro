pluginName = SINWAV

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += SINWAV.h

SOURCES += SINWAV.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target

