!include( ../../../misc.plugin.config ){
  message( "Oops -- No custom build options specified" )
}

HEADERS += Alert.h
HEADERS += AlertConfig.h
HEADERS += AlertConfigureDialog.h
HEADERS += AlertDataBase.h
HEADERS += AlertDialog.h
HEADERS += AlertEditDialog.h
HEADERS += AlertItem.h
HEADERS += AlertScript.h
HEADERS += AlertThread.h

SOURCES += Alert.cpp
SOURCES += AlertConfig.cpp
SOURCES += AlertConfigureDialog.cpp
SOURCES += AlertDataBase.cpp
SOURCES += AlertDialog.cpp
SOURCES += AlertEditDialog.cpp
SOURCES += AlertItem.cpp
SOURCES += AlertScript.cpp
SOURCES += AlertThread.cpp

QT += phonon
