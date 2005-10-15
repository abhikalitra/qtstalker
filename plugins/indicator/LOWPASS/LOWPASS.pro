pluginName = LOWPASS

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += LOWPASS.h
HEADERS += qtsFFT.h

SOURCES += LOWPASS.cpp
SOURCES += qtsFFT.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
