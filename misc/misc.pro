TEMPLATE = app

CONFIG += qt thread warn_on debug
CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += CSV.h
HEADERS += Bar.h

SOURCES += main.cpp
SOURCES += CSV.cpp
SOURCES += Bar.cpp

TARGET = CSV

unix:linux-g++:LIBS += -ldl

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

QT += sql


