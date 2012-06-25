TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += ControlWidget.h
SOURCES += ControlWidget.cpp
HEADERS += Indicator.h
SOURCES += Indicator.cpp
HEADERS += IndicatorDialog.h
SOURCES += IndicatorDialog.cpp
HEADERS += LengthButton.h
SOURCES += LengthButton.cpp
HEADERS += PlotWidget.h
SOURCES += PlotWidget.cpp
HEADERS += RangeButton.h
SOURCES += RangeButton.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
