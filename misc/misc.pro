TEMPLATE = app

!win32 {
  CONFIG += qt thread warn_on debug
} else {
  CONFIG += thread warn_on debug_and_release
  CONFIG(debug, debug|release): DESTDIR = ../mingwin32/deploydebug
  CONFIG(release, debug|release): DESTDIR = ../mingwin32/deploy
  DEFINES += MINGW
}

CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math

HEADERS += MainWindow.h
HEADERS += CSVRuleDialog.h
HEADERS += CSV.h
HEADERS += CSVRule.h
HEADERS += Bar.h

SOURCES += MainWindow.cpp
SOURCES += main.cpp
SOURCES += CSVRuleDialog.cpp
SOURCES += CSV.cpp
SOURCES += CSVRule.cpp
SOURCES += Bar.cpp

TARGET = CSV

win32 {
  CONFIG(debug, debug|release): TARGET = qtstalkerD
}

win32 {
  exists(../qtc-gdbmacros/gdbmacros.cpp) {
    CONFIG(debug, debug|release): SOURCES += ../qtc-gdbmacros/gdbmacros.cpp
  }

  CONFIG(debug, debug|release): LIBS += -L../mingwin32/deploydebug -lqtstalkerD0
  CONFIG(release, debug|release): LIBS += -L../mingwin32/deploy -lqtstalker0
}

unix:linux-g++:LIBS += -ldl

!win32 {
  target.path = "$${INSTALL_BIN_DIR}"
  INSTALLS += target
}

unix:linux-g++:LIBS += -ldl

QT += sql


