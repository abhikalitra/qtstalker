TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += MySQLPlugin.h

SOURCES += MySQLPlugin.cpp

TARGET = MySQL.0.30

INCLUDEPATH += ../../../lib
INCLUDEPATH += /usr/include/mysql

LIBS += -lmysqlclient
LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

# for redhat
LIBS += -L/usr/lib/mysql

# for 64bit systems
LIBS += -L/usr/lib64/mysql

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/quote
INSTALLS += target

