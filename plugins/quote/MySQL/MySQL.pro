TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += MySQLPlugin.h

SOURCES += MySQLPlugin.cpp

TARGET = MySQL.0.26

INCLUDEPATH += ../../../lib
INCLUDEPATH += /usr/include/mysql
INCLUDEPATH += ../../../db-2.7.7/build_unix

LIBS += -lmysqlclient

# for redhat
LIBS += -L/usr/lib/mysql

unix:linux-g++:LIBS += -ldl

target.path = /usr/lib/qtstalker/quote
INSTALLS += target

