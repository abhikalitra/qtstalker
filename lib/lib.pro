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
HEADERS += IndicatorDialog.h

HEADERS += SCGetIndicator.h
HEADERS += SCSetIndicator.h
HEADERS += SCIndicator.h
HEADERS += SCPlot.h
HEADERS += SCSymbolList.h
HEADERS += ACOS.h
HEADERS += AD.h
HEADERS += ADD.h
HEADERS += ADX.h
HEADERS += APO.h
HEADERS += AROON.h
HEADERS += ASIN.h
HEADERS += ATAN.h
HEADERS += AVGPRICE.h
HEADERS += BARS.h
HEADERS += BBANDS.h
HEADERS += BETA.h
HEADERS += BOP.h
HEADERS += CANDLES.h
HEADERS += CCI.h
HEADERS += CDL.h
HEADERS += CEIL.h
HEADERS += CMO.h
HEADERS += COLOR.h
HEADERS += COMPARE.h
HEADERS += CORREL.h
HEADERS += COS.h
HEADERS += DEMA.h
HEADERS += DIV.h
HEADERS += DX.h
HEADERS += EMA.h
HEADERS += EXP.h
HEADERS += FLOOR.h
HEADERS += HT_DCPERIOD.h
HEADERS += HT_DCPHASE.h
HEADERS += HT_PHASOR.h
HEADERS += HT_SINE.h
HEADERS += HT_TRENDLINE.h
HEADERS += HT_TRENDMODE.h
HEADERS += KAMA.h
HEADERS += LINEARREG.h
HEADERS += LINEARREG_ANGLE.h
HEADERS += LINEARREG_INTERCEPT.h
HEADERS += LINEARREG_SLOPE.h
HEADERS += LN.h
HEADERS += LOG10.h
HEADERS += MACD.h
HEADERS += MAMA.h
HEADERS += MAVP.h
HEADERS += MAX.h
HEADERS += MAXINDEX.h
HEADERS += MEDPRICE.h
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
HEADERS += MULT.h
HEADERS += NATR.h
HEADERS += NORMAL.h
HEADERS += OBV.h
HEADERS += PLUS_DI.h
HEADERS += PLUS_DM.h
HEADERS += PPO.h
HEADERS += REF.h
HEADERS += ROC.h
HEADERS += RSI.h
HEADERS += SAR.h
HEADERS += SIN.h
HEADERS += SMA.h
HEADERS += SQRT.h
HEADERS += STDDEV.h
HEADERS += STOCH.h
HEADERS += SUB.h
HEADERS += SUM.h
HEADERS += SYMBOL.h
HEADERS += T3.h
HEADERS += TAN.h
HEADERS += TEMA.h
HEADERS += TRANGE.h
HEADERS += TRIMA.h
HEADERS += TRIX.h
HEADERS += TSF.h
HEADERS += TYPPRICE.h
HEADERS += ULTOSC.h
HEADERS += VAR.h
HEADERS += WCLPRICE.h
HEADERS += WILLR.h
HEADERS += WMA.h


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
SOURCES += IndicatorDialog.cpp

SOURCES += SCGetIndicator.cpp
SOURCES += SCSetIndicator.cpp
SOURCES += SCIndicator.cpp
SOURCES += SCPlot.cpp
SOURCES += SCSymbolList.cpp
SOURCES += ACOS.cpp
SOURCES += AD.cpp
SOURCES += ADD.cpp
SOURCES += ADX.cpp
SOURCES += APO.cpp
SOURCES += AROON.cpp
SOURCES += ASIN.cpp
SOURCES += ATAN.cpp
SOURCES += AVGPRICE.cpp
SOURCES += BARS.cpp
SOURCES += BBANDS.cpp
SOURCES += BETA.cpp
SOURCES += BOP.cpp
SOURCES += CANDLES.cpp
SOURCES += CCI.cpp
SOURCES += CDL.cpp
SOURCES += CEIL.cpp
SOURCES += CMO.cpp
SOURCES += COLOR.cpp
SOURCES += COMPARE.cpp
SOURCES += CORREL.cpp
SOURCES += COS.cpp
SOURCES += DEMA.cpp
SOURCES += DIV.cpp
SOURCES += DX.cpp
SOURCES += EMA.cpp
SOURCES += EXP.cpp
SOURCES += FLOOR.cpp
SOURCES += HT_DCPERIOD.cpp
SOURCES += HT_DCPHASE.cpp
SOURCES += HT_PHASOR.cpp
SOURCES += HT_SINE.cpp
SOURCES += HT_TRENDLINE.cpp
SOURCES += HT_TRENDMODE.cpp
SOURCES += KAMA.cpp
SOURCES += LINEARREG.cpp
SOURCES += LINEARREG_ANGLE.cpp
SOURCES += LINEARREG_INTERCEPT.cpp
SOURCES += LINEARREG_SLOPE.cpp
SOURCES += LN.cpp
SOURCES += LOG10.cpp
SOURCES += MACD.cpp
SOURCES += MAMA.cpp
SOURCES += MAVP.cpp
SOURCES += MAX.cpp
SOURCES += MAXINDEX.cpp
SOURCES += MEDPRICE.cpp
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
SOURCES += MULT.cpp
SOURCES += NATR.cpp
SOURCES += NORMAL.cpp
SOURCES += OBV.cpp
SOURCES += PLUS_DI.cpp
SOURCES += PLUS_DM.cpp
SOURCES += PPO.cpp
SOURCES += REF.cpp
SOURCES += ROC.cpp
SOURCES += RSI.cpp
SOURCES += SAR.cpp
SOURCES += SIN.cpp
SOURCES += SMA.cpp
SOURCES += SQRT.cpp
SOURCES += STDDEV.cpp
SOURCES += STOCH.cpp
SOURCES += SUB.cpp
SOURCES += SUM.cpp
SOURCES += SYMBOL.cpp
SOURCES += T3.cpp
SOURCES += TAN.cpp
SOURCES += TEMA.cpp
SOURCES += TRANGE.cpp
SOURCES += TRIMA.cpp
SOURCES += TRIX.cpp
SOURCES += TSF.cpp
SOURCES += TYPPRICE.cpp
SOURCES += ULTOSC.cpp
SOURCES += VAR.cpp
SOURCES += WCLPRICE.cpp
SOURCES += WILLR.cpp
SOURCES += WMA.cpp


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



