TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += Stocks.h
HEADERS += StocksDialog.h

SOURCES += Stocks.cpp
SOURCES += StocksDialog.cpp

TARGET = Stocks.0.27

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/db
INSTALLS += target
