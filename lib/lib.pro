TEMPLATE = lib

CONFIG += thread warn_on debug
CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -rdynamic -ffast-math -O2

HEADERS += Setting.h
HEADERS += PlotLine.h
HEADERS += BarData.h
HEADERS += Scaler.h
HEADERS += PrefDialog.h
HEADERS += ColorButton.h
HEADERS += FontButton.h
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
HEADERS += SCGetIndicator.h
HEADERS += SCSetIndicator.h
HEADERS += SCPlot.h
HEADERS += SCSymbolList.h
HEADERS += IndicatorBase.h
HEADERS += IndicatorFactory.h

HEADERS += AD.h
HEADERS += ADOSC.h
HEADERS += ADX.h
HEADERS += AROON.h
HEADERS += ATR.h
HEADERS += BARS.h
HEADERS += BBANDS.h
HEADERS += BETA.h
HEADERS += BOP.h
HEADERS += CANDLES.h
HEADERS += CCI.h
HEADERS += CMO.h
HEADERS += COLOR.h
HEADERS += COMPARE.h
HEADERS += CORREL.h
HEADERS += CUS.h
HEADERS += FI.h
HEADERS += HT.h
HEADERS += HT_PHASOR.cpp
HEADERS += HT_SINE.cpp
HEADERS += LINEARREG.h
HEADERS += LMS.h
HEADERS += MA.h
HEADERS += MACD.h
HEADERS += MAMA.h
HEADERS += MATH1.h
HEADERS += MATH2.h
HEADERS += MAVP.h
HEADERS += MFI.h
HEADERS += MOM.h
HEADERS += PP.h
HEADERS += PO.h
HEADERS += RANGE.h
HEADERS += REF.h
HEADERS += ROC.h
HEADERS += RSI.h
HEADERS += SAR.h
HEADERS += SINWAV.h
HEADERS += STDDEV.h
HEADERS += STOCH.h
HEADERS += STOCHRSI.h
HEADERS += SUM.h
HEADERS += SYMBOL.h
HEADERS += SZ.h
HEADERS += T3.h
HEADERS += THERM.h
HEADERS += TRIX.h
HEADERS += TSF.h
HEADERS += ULTOSC.h
HEADERS += VAR.h
HEADERS += VFI.h
HEADERS += VIDYA.h
HEADERS += VOL.h
HEADERS += WILLR.h


SOURCES += Setting.cpp
SOURCES += PlotLine.cpp
SOURCES += BarData.cpp
SOURCES += Scaler.cpp
SOURCES += PrefDialog.cpp
SOURCES += ColorButton.cpp
SOURCES += FontButton.cpp
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
SOURCES += SCGetIndicator.cpp
SOURCES += SCSetIndicator.cpp
SOURCES += SCPlot.cpp
SOURCES += SCSymbolList.cpp
SOURCES += IndicatorBase.cpp
SOURCES += IndicatorFactory.cpp

SOURCES += AD.cpp
SOURCES += ADOSC.cpp
SOURCES += ADX.cpp
SOURCES += AROON.cpp
SOURCES += ATR.cpp
SOURCES += BARS.cpp
SOURCES += BBANDS.cpp
SOURCES += BETA.cpp
SOURCES += BOP.cpp
SOURCES += CANDLES.cpp
SOURCES += CCI.cpp
SOURCES += CMO.cpp
SOURCES += COLOR.cpp
SOURCES += COMPARE.cpp
SOURCES += CORREL.cpp
SOURCES += CUS.cpp
SOURCES += FI.cpp
SOURCES += HT.cpp
SOURCES += HT_PHASOR.cpp
SOURCES += HT_SINE.cpp
SOURCES += LINEARREG.cpp
SOURCES += LMS.cpp
SOURCES += MA.cpp
SOURCES += MACD.cpp
SOURCES += MAMA.cpp
SOURCES += MATH1.cpp
SOURCES += MATH2.cpp
SOURCES += MAVP.cpp
SOURCES += MFI.cpp
SOURCES += MOM.cpp
SOURCES += PP.cpp
SOURCES += PO.cpp
SOURCES += RANGE.cpp
SOURCES += REF.cpp
SOURCES += ROC.cpp
SOURCES += RSI.cpp
SOURCES += SAR.cpp
SOURCES += SINWAV.cpp
SOURCES += STDDEV.cpp
SOURCES += STOCH.cpp
SOURCES += STOCHRSI.cpp
SOURCES += SUM.cpp
SOURCES += SYMBOL.cpp
SOURCES += SZ.cpp
SOURCES += T3.cpp
SOURCES += THERM.cpp
SOURCES += TRIX.cpp
SOURCES += TSF.cpp
SOURCES += ULTOSC.cpp
SOURCES += VAR.cpp
SOURCES += VFI.cpp
SOURCES += VIDYA.cpp
SOURCES += VOL.cpp
SOURCES += WILLR.cpp


TARGET = qtstalker

VERSION = 0.37.0

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

unix:linux-g++:LIBS += -ldl

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

target.path = "$${INSTALL_LIB_DIR}"
INSTALLS += target

# FIXME: QMAKE_DISTCLEAN does not seem to work from the top-level qtstalker.pro
# so doing it here.
QMAKE_DISTCLEAN += qtstalker_defines.h

QT += sql



