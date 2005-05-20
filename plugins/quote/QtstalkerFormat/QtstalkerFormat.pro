pluginName = QtstalkerFormat

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += QtstalkerFormat.h

SOURCES += QtstalkerFormat.cpp

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
