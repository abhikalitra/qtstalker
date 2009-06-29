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

HEADERS += Qtstalker.h

SOURCES += Qtstalker.cpp
SOURCES += main.cpp

TARGET = qtstalker

win32 {
  CONFIG(debug, debug|release): TARGET = qtstalkerD
}

INCLUDEPATH += ../lib

win32 {
  exists(../qtc-gdbmacros/gdbmacros.cpp) {
    CONFIG(debug, debug|release): SOURCES += ../qtc-gdbmacros/gdbmacros.cpp
  }

  INCLUDEPATH += ../mingwin32/ta-lib/include
  CONFIG(debug, debug|release): LIBS += -L../mingwin32/deploydebug -lqtstalkerD0
  CONFIG(release, debug|release): LIBS += -L../mingwin32/deploy -lqtstalker0
} else {
  LIBS += -L../lib -lqtstalker
}

unix:linux-g++:LIBS += -ldl

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

!win32 {
  target.path = "$${INSTALL_BIN_DIR}"
  INSTALLS += target
}

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += ../.qmake.cache

