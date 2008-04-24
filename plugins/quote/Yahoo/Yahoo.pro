pluginName = Yahoo

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Yahoo.h

SOURCES += Yahoo.cpp

target.path = /usr/local/lib/qtstalker/quote
INSTALLS += target
#The following line was inserted by qt3to4
QT +=  qt3support 
