TEMPLATE = lib

CONFIG += thread warn_on plugin
QMAKE_CXXFLAGS += -Os

HEADERS += TRIX.h

SOURCES += TRIX.cpp

TARGET = TRIX

INCLUDEPATH += ../../../lib

#LIBS += -L/usr/lib
unix:linux-g++:LIBS += -ldl

# mac os x options
macx:QMAKE_CXXFLAGS += -bundle

target.path = /usr/lib/qtstalker/indicator
INSTALLS += target
