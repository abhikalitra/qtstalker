!include( ../../../misc.plugin.config ){
  message( "Oops -- No custom build options specified" )
}

HEADERS += Alert.h
HEADERS += AlertConfig.h
HEADERS += AlertDataBase.h
#HEADERS += CSVDeleteDialog.h
HEADERS += AlertDialog.h
HEADERS += AlertEditDialog.h
HEADERS += AlertItem.h
#HEADERS += CSVNewDialog.h
#HEADERS += CSVRule.h
#HEADERS += CSVRuleDialog.h
#HEADERS += CSVRunDialog.h
HEADERS += AlertOperator.h
HEADERS += AlertThread.h

SOURCES += Alert.cpp
SOURCES += AlertConfig.cpp
SOURCES += AlertDataBase.cpp
#SOURCES += CSVDeleteDialog.cpp
SOURCES += AlertDialog.cpp
SOURCES += AlertEditDialog.cpp
SOURCES += AlertItem.cpp
#SOURCES += CSVNewDialog.cpp
#SOURCES += CSVRule.cpp
#SOURCES += CSVRuleDialog.cpp
#SOURCES += CSVRunDialog.cpp
SOURCES += AlertOperator.cpp
SOURCES += AlertThread.cpp
