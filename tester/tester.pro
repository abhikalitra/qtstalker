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
HEADERS += TestDataBase.h
HEADERS += TestTrade.h
HEADERS += TestSignal.h

SOURCES += QtStalkerTester.cpp
SOURCES += TestConfig.cpp
SOURCES += TestRankings.cpp
SOURCES += TestSettings.cpp
SOURCES += TestReport.cpp
SOURCES += TestChart.cpp
SOURCES += TestDataBase.cpp
SOURCES += TestTrade.cpp
SOURCES += TestSignal.cpp
SOURCES += main.cpp

TARGET = QtStalkerTester

INCLUDEPATH += ../lib

LIBS += -L../lib -lqtstalker

unix:linux-g++:LIBS += -ldl

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_BIN_DIR}"
INSTALLS += target

QT += sql

