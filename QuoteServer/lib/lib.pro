TEMPLATE = lib

CONFIG += qt thread warn_on
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2


HEADERS += Globals.h
HEADERS += Symbol.h
HEADERS += Bar.h
HEADERS += DataBase.h
HEADERS += QuoteDataBase.h
HEADERS += RequestThread.h
HEADERS += Plugin.h
HEADERS += PluginFactory.h
HEADERS += QSLog.h


SOURCES += Globals.cpp
SOURCES += Bar.cpp
SOURCES += DataBase.cpp
SOURCES += QuoteDataBase.cpp
SOURCES += RequestThread.cpp
SOURCES += Plugin.cpp
SOURCES += PluginFactory.cpp
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


