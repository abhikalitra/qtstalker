pluginName = MA

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MA.h
HEADERS += qtsFFT.h
HEADERS += MADialog.h

SOURCES += MA.cpp
SOURCES += qtsFFT.cpp
SOURCES += MADialog.cpp

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
