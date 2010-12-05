!include( ../../../script.plugin.config ){
  message( "Oops -- No custom build options specified" )
}

HEADERS += QUOTE_DATABASE.h
HEADERS += QuoteBar.h
HEADERS += DateRange.h
HEADERS += Symbol.h

SOURCES += QUOTE_DATABASE.cpp
SOURCES += QuoteBar.cpp
SOURCES += DateRange.cpp
SOURCES += Symbol.cpp
