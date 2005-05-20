pluginName = Cycle

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Cycle.h
HEADERS += CycleObject.h

SOURCES += Cycle.cpp
SOURCES += CycleObject.cpp

target.path = /usr/lib/qtstalker/co
INSTALLS += target
