TEMPLATE = lib

CONFIG += qt thread warn_on
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2


HEADERS += QSGlobals.h
HEADERS += QSSymbol.h
HEADERS += QSBar.h
HEADERS += QSDataBase.h
HEADERS += QSQuoteDataBase.h
HEADERS += QSRequestThread.h
HEADERS += QSPlugin.h
HEADERS += QSPluginFactory.h
HEADERS += QSLog.h


SOURCES += QSGlobals.cpp
SOURCES += QSBar.cpp
SOURCES += QSDataBase.cpp
SOURCES += QSQuoteDataBase.cpp
SOURCES += QSRequestThread.cpp
SOURCES += QSPlugin.cpp
SOURCES += QSPluginFactory.cpp
SOURCES += QSLog.cpp


VERSION = 0.1.0

unix {
  # qmake automatically adds /usr/lib
  INCLUDEPATH += /usr/include/qt4/Qt
  exists(/usr/local/lib) {
    LIBS += -L/usr/local/lib
  }
}


TARGET = QuoteServer
target.path = /usr/local/lib
INSTALLS += target


QT -= gui
QT += core
QT += network
QT += sql


