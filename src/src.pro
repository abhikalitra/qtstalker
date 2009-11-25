TEMPLATE = app

CONFIG += qt thread warn_on debug
CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += Qtstalker.h
HEADERS += assistant.h

SOURCES += Qtstalker.cpp
SOURCES += main.cpp
SOURCES += assistant.cpp

TARGET = qtstalker

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += ../.qmake.cache

