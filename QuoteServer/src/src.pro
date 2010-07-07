TEMPLATE = app

CONFIG += qt thread warn_on

QMAKE_CXXFLAGS += -rdynamic -ffast-math


HEADERS += QuoteServer.h

SOURCES += main.cpp
SOURCES += QuoteServer.cpp


unix {
  # qmake automatically adds /usr/lib
  INCLUDEPATH += /usr/include/qt4/Qt
  exists(/usr/local/lib) {
    LIBS += -L/usr/local/lib
  }
}

INCLUDEPATH += ../lib

LIBS += -L../lib -lQuoteServer


TARGET = QuoteServer
target.path = /usr/local/bin
INSTALLS += target



# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += ../.qmake.cache

QT -= gui
QT += core
QT += network

