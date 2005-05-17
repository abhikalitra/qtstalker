TEMPLATE = lib

CONFIG += thread warn_on plugin

QMAKE_CXXFLAGS += -ffast-math -Os

HEADERS += BuyArrow.h
HEADERS += BuyArrowObject.h

SOURCES += BuyArrow.cpp
SOURCES += BuyArrowObject.cpp

TARGET = BuyArrow.0.31

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker
LIBS += -ldb

unix:linux-g++:LIBS += -ldl

# FreeBSD options
unix:freebsd-g++:INCLUDEPATH += /usr/X11R6/include
unix:freebsd-g++:INCLUDEPATH += /usr/local/include/db42
unix:freebsd-g++:LIBS += -L/usr/local/lib/db42

target.path = /usr/lib/qtstalker/co
INSTALLS += target
