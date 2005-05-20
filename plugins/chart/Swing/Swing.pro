pluginName = Swing

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Swing.h

SOURCES += Swing.cpp

target.path = /usr/lib/qtstalker/chart
INSTALLS += target
