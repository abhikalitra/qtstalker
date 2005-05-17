TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += CC.h

SOURCES += CC.cpp

TARGET = CC.0.31

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

# FreeBSD options
unix:freebsd-g++:INCLUDEPATH += /usr/X11R6/include
unix:freebsd-g++:INCLUDEPATH += /usr/local/include/db42
unix:freebsd-g++:LIBS += -L/usr/local/lib/db42

target.path = /usr/lib/qtstalker/db
INSTALLS += target
