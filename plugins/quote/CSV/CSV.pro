pluginName = CSV

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += CSV.h
HEADERS += CSVDialog.h
HEADERS += CSVRuleDialog.h

SOURCES += CSV.cpp
SOURCES += CSVDialog.cpp
SOURCES += CSVRuleDialog.cpp

target.path = /usr/lib/qtstalker/quote
INSTALLS += target
