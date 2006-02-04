pluginName = TALIB

!include( ../../../plugin.config ){
  message( "Oops -- No custom build options specified" ) 
} 

HEADERS += TALIB.h

SOURCES += TALIB.cpp

LIBS += -lta_libc_cmr

unix:INCLUDEPATH += ta-lib/c/include
unix:LIBS += -Lta-lib/c/lib

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target

system(tar zxvf talib.tar.gz)
unix:linux-g++:system(cd ta-lib/c/make/cmr/linux/g++ ; make)
unix:freebsd-g++:system(cd ta-lib/c/make/cmr/freebsd/g++ ; make)
