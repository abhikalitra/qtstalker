TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += Stocks.h
HEADERS += StocksDialog.h

SOURCES += Stocks.cpp
SOURCES += StocksDialog.cpp

TARGET = Stocks.0.30

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/db
INSTALLS += target
