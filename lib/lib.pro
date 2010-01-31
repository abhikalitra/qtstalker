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
HEADERS += APO.h
HEADERS += AROON.h
HEADERS += AROONOSC.h
HEADERS += ATR.h
HEADERS += BARS.h
HEADERS += BBANDS.h
HEADERS += BETA.h
HEADERS += BOP.h
HEADERS += CANDLES.h
HEADERS += CCI.h
HEADERS += CDL.h
HEADERS += CMO.h
HEADERS += COLOR.h
HEADERS += COMPARE.h
HEADERS += CORREL.h
HEADERS += CUS.h
HEADERS += DEMA.h
HEADERS += DX.h
HEADERS += FI.h
HEADERS += HT_DCPERIOD.h
HEADERS += HT_DCPHASE.h
HEADERS += HT_PHASOR.h
HEADERS += HT_SINE.h
HEADERS += HT_TRENDLINE.h
HEADERS += HT_TRENDMODE.h
HEADERS += LINEARREG.h
HEADERS += LINEARREG_ANGLE.h
HEADERS += LINEARREG_INTERCEPT.h
HEADERS += LINEARREG_SLOPE.h
HEADERS += LMS.h
HEADERS += MA.h
HEADERS += MACD.h
HEADERS += MACDEXT.h
HEADERS += MACDFIX.h
HEADERS += MAMA.h
HEADERS += MATH.h
HEADERS += MATH2.h
HEADERS += MAVP.h
HEADERS += MAX.h
HEADERS += MAXINDEX.h
HEADERS += MFI.h
HEADERS += MIDPOINT.h
HEADERS += MIDPRICE.h
HEADERS += MIN.h
HEADERS += MININDEX.h
HEADERS += MINMAX.h
HEADERS += MINMAXINDEX.h
HEADERS += MINUS_DI.h
HEADERS += MINUS_DM.h
HEADERS += MOM.h
HEADERS += NATR.h
HEADERS += PLUS_DI.h
HEADERS += PLUS_DM.h
HEADERS += PP.h
HEADERS += PPO.h
HEADERS += PRICE.h
HEADERS += REF.h
HEADERS += ROC.h
HEADERS += RSI.h
HEADERS += SAR.h
HEADERS += SAREXT.h
HEADERS += SINWAV.h
HEADERS += STDDEV.h
HEADERS += STOCH.h
HEADERS += STOCHF.h
HEADERS += STOCHRSI.h
HEADERS += SUM.h
HEADERS += SYMBOL.h
HEADERS += SZ.h
HEADERS += T3.h
HEADERS += THERM.h
HEADERS += TRANGE.h
HEADERS += TRIG.h
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
SOURCES += APO.cpp
SOURCES += AROON.cpp
SOURCES += AROONOSC.cpp
SOURCES += ATR.cpp
SOURCES += BARS.cpp
SOURCES += BBANDS.cpp
SOURCES += BETA.cpp
SOURCES += BOP.cpp
SOURCES += CANDLES.cpp
SOURCES += CCI.cpp
SOURCES += CDL.cpp
SOURCES += CMO.cpp
SOURCES += COLOR.cpp
SOURCES += COMPARE.cpp
SOURCES += CORREL.cpp
SOURCES += CUS.cpp
SOURCES += DEMA.cpp
SOURCES += DX.cpp
SOURCES += FI.cpp
SOURCES += HT_DCPERIOD.cpp
SOURCES += HT_DCPHASE.cpp
SOURCES += HT_PHASOR.cpp
SOURCES += HT_SINE.cpp
SOURCES += HT_TRENDLINE.cpp
SOURCES += HT_TRENDMODE.cpp
SOURCES += LINEARREG.cpp
SOURCES += LINEARREG_ANGLE.cpp
SOURCES += LINEARREG_INTERCEPT.cpp
SOURCES += LINEARREG_SLOPE.cpp
SOURCES += LMS.cpp
SOURCES += MA.cpp
SOURCES += MACD.cpp
SOURCES += MACDEXT.cpp
SOURCES += MACDFIX.cpp
SOURCES += MAMA.cpp
SOURCES += MATH.cpp
SOURCES += MATH2.cpp
SOURCES += MAVP.cpp
SOURCES += MAX.cpp
SOURCES += MAXINDEX.cpp
SOURCES += MFI.cpp
SOURCES += MIDPOINT.cpp
SOURCES += MIDPRICE.cpp
SOURCES += MIN.cpp
SOURCES += MININDEX.cpp
SOURCES += MINMAX.cpp
SOURCES += MINMAXINDEX.cpp
SOURCES += MINUS_DI.cpp
SOURCES += MINUS_DM.cpp
SOURCES += MOM.cpp
SOURCES += NATR.cpp
SOURCES += PLUS_DI.cpp
SOURCES += PLUS_DM.cpp
SOURCES += PP.cpp
SOURCES += PPO.cpp
SOURCES += PRICE.cpp
SOURCES += REF.cpp
SOURCES += ROC.cpp
SOURCES += RSI.cpp
SOURCES += SAR.cpp
SOURCES += SAREXT.cpp
SOURCES += SINWAV.cpp
SOURCES += STDDEV.cpp
SOURCES += STOCH.cpp
SOURCES += STOCHF.cpp
SOURCES += STOCHRSI.cpp
SOURCES += SUM.cpp
SOURCES += SYMBOL.cpp
SOURCES += SZ.cpp
SOURCES += T3.cpp
SOURCES += THERM.cpp
SOURCES += TRANGE.cpp
SOURCES += TRIG.cpp
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



