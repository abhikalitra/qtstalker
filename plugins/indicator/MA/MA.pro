pluginName = MA

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MA.h
HEADERS += qtsFFT.h

SOURCES += MA.cpp
SOURCES += qtsFFT.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
