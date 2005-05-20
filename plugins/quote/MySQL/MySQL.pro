pluginName = MySQL

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += MySQLPlugin.h

SOURCES += MySQLPlugin.cpp

INCLUDEPATH += /usr/include/mysql

LIBS += -lmysqlclient

# for redhat
LIBS += -L/usr/lib/mysql

# for 64bit systems
LIBS += -L/usr/lib64/mysql

target.path = /usr/lib/qtstalker/quote
INSTALLS += target

