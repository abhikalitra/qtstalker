pluginName = CME

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CME.h

SOURCES += CME.cpp

target.path = /usr/local/lib/qtstalker/quote
INSTALLS += target
#The following line was inserted by qt3to4
QT +=  qt3support 
