TEMPLATE = lib

!win32 {
	CONFIG += thread warn_on debug
} else {
	CONFIG += thread warn_on debug_and_release
	CONFIG(debug, debug|release): DESTDIR = ../mingwin32/deploydebug
	CONFIG(release, debug|release): DESTDIR = ../mingwin32/deploy
        DEFINES += MINGW
}

CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += Setting.h
HEADERS += PlotLine.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += PrefDialog.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
#HEADERS += FileButton.h
HEADERS += Bar.h
HEADERS += Config.h
HEADERS += Plot.h
HEADERS += DatePlot.h
HEADERS += ScalePlot.h
HEADERS += IndicatorPlot.h
HEADERS += DataWindow.h
HEADERS += Preferences.h
HEADERS += DataBase.h
HEADERS += Setup.h
HEADERS += ChartPage.h
HEADERS += GroupPage.h
HEADERS += IndicatorPage.h
HEADERS += COSettings.h
HEADERS += ExScript.h
HEADERS += Indicator.h
HEADERS += CODialog.h


   
SOURCES += Setting.cpp
SOURCES += PlotLine.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += PrefDialog.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
#SOURCES += FileButton.cpp
SOURCES += Bar.cpp
SOURCES += Config.cpp
SOURCES += Plot.cpp
SOURCES += DatePlot.cpp
SOURCES += ScalePlot.cpp
SOURCES += IndicatorPlot.cpp
SOURCES += DataWindow.cpp
SOURCES += Preferences.cpp
SOURCES += DataBase.cpp
SOURCES += Setup.cpp
SOURCES += ChartPage.cpp
SOURCES += GroupPage.cpp
SOURCES += IndicatorPage.cpp
SOURCES += COSettings.cpp
SOURCES += ExScript.cpp
SOURCES += Indicator.cpp
SOURCES += CODialog.cpp


   
TARGET = qtstalker

win32 {
	CONFIG(debug, debug|release): TARGET = qtstalkerD
}

VERSION = 0.37.0

unix {
  # qmake automatically adds /usr/lib
  INCLUDEPATH += /usr/include/qt4/Qt
  exists(/usr/local/lib) {
    LIBS += -L/usr/local/lib
  }
}

win32 {
    INCLUDEPATH += ../mingwin32/ta-lib/include
    LIBS += -L../mingwin32/ta-lib/lib
	LIBS += -lta_lib
	LIBS += -lwsock32
  } else {
  TA_LIB_VERSION = $$system(ta-lib-config --version)
  contains(TA_LIB_VERSION, 0.3.0) {
    LIBS += -lta_abstract
    LIBS += -lta_common
    LIBS += -lta_func
  } else {
    LIBS += -lta_lib
  }
}

unix:linux-g++:LIBS += -ldl

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

!win32 {
	target.path = "$${INSTALL_LIB_DIR}"
	INSTALLS += target
}

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += qtstalker_defines.h

QT += sql



