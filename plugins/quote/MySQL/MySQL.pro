TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += MySQLPlugin.h

SOURCES += MySQLPlugin.cpp

TARGET = MySQL

INCLUDEPATH += ../../../db-2.7.7/build_unix
INCLUDEPATH += ../../../lib
INCLUDEPATH += /usr/include/mysql

LIBS += -L../../../db-2.7.7/build_unix -ldb
LIBS += -L/usr/lib
LIBS += -lmysqlclient

# for redhat
LIBS += -L/usr/lib/mysql

unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

target.path = /usr/lib/qtstalker/quote
INSTALLS += target

