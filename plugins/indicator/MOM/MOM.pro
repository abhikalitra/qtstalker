pluginName = MOM

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MOM.h

SOURCES += MOM.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
