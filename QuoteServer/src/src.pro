TEMPLATE = app

CONFIG += qt thread warn_on

QMAKE_CXXFLAGS += -rdynamic -ffast-math


HEADERS += QuoteServer.h

SOURCES += main.cpp
SOURCES += QuoteServer.cpp

INCLUDEPATH += ../lib

LIBS += -L../lib -lQuoteServer


TARGET = QuoteServer
target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target


# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += ../.qmake.cache

QT -= gui
QT += core
QT += network

