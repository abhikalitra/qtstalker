TEMPLATE = lib

CONFIG += plugin
CONFIG += thread warn_on debug

INCLUDEPATH += ../../../lib

LIBS += -L../../../lib -lqtstalker

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += VOL.h

SOURCES += VOL.cpp

unix {
  # qmake automatically adds /usr/lib
  INCLUDEPATH += /usr/include/qt4/Qt
  exists(/usr/local/lib) {
    LIBS += -L/usr/local/lib
  }
}

TA_LIB_VERSION = $$system(ta-lib-config --version)
contains(TA_LIB_VERSION, 0.3.0) {
    LIBS += -lta_abstract
    LIBS += -lta_common
    LIBS += -lta_func
  } else {
    LIBS += -lta_lib
  }

target.path = /usr/local/lib/qtstalker/plugins/indicator
INSTALLS += target

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
#QMAKE_DISTCLEAN += qtstalker_defines.h
QMAKE_DISTCLEAN += ../../../.qmake.cache

