pluginName = CSV

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CSV.h
HEADERS += CSVRuleDialog.h

SOURCES += CSV.cpp
SOURCES += CSVRuleDialog.cpp

target.path = /usr/local/lib/qtstalker/quote
INSTALLS += target
#The following line was inserted by qt3to4
QT +=  qt3support 
