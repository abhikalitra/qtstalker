pluginName = Index

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += Index.h
HEADERS += IndexDialog.h

SOURCES += Index.cpp
SOURCES += IndexDialog.cpp

target.path = /usr/lib/qtstalker/db
INSTALLS += target
