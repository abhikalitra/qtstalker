pluginName = ROC

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += ROC.h

SOURCES += ROC.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
