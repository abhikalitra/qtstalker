pluginName = LMS

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += LMS.h

SOURCES += LMS.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
