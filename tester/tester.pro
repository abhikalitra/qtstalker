TEMPLATE = app

CONFIG += qt thread warn_on
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += QtStalkerTester.h
HEADERS += TestConfig.h
HEADERS += TestRankings.h
HEADERS += TestSettings.h
HEADERS += TestReport.h
HEADERS += TestChart.h

SOURCES += QtStalkerTester.cpp
SOURCES += TestConfig.cpp
SOURCES += TestRankings.cpp
SOURCES += TestSettings.cpp
SOURCES += TestReport.cpp
SOURCES += TestChart.cpp
SOURCES += main.cpp

TARGET = QtStalkerTester

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

QT += sql

