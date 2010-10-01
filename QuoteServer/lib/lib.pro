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
HEADERS += QSDateRange.h
HEADERS += QSDetailsDataBase.h
HEADERS += QSDetail.h


SOURCES += QSGlobals.cpp
SOURCES += QSBar.cpp
SOURCES += QSDataBase.cpp
SOURCES += QSQuoteDataBase.cpp
SOURCES += QSRequestThread.cpp
SOURCES += QSPlugin.cpp
SOURCES += QSPluginFactory.cpp
SOURCES += QSLog.cpp
SOURCES += QSDateRange.cpp
SOURCES += QSDetailsDataBase.cpp
SOURCES += QSDetail.cpp


VERSION = 0.1.0

TARGET = QuoteServer
target.path = "$${INSTALL_LIB_DIR}"
INSTALLS += target


QT -= gui
QT += core
QT += network
QT += sql


