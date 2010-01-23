/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#ifndef SCINDICATOR_HPP
#define SCINDICATOR_HPP


#include <QStringList>
#include <QString>
#include <QHash>

#include "PlotLine.h"
#include "BarData.h"


class SCIndicator
{
  public:
    enum IndicatorName
    {
      _ACOS,
      _AD,
      _ADD,
      _ADOSC,
      _ADX,
      _ADXR,
      _APO,
      _AROON,
      _AROONOSC,
      _ASIN,
      _ATAN,
      _ATR,
      _AVGPRICE,
      _BARS,
      _BBANDS,
      _BETA,
      _BOP,
      _CANDLES,
      _CCI,
      _CDL2CROWS,
      _CDL3BLACKCROWS,
      _CDL3INSIDE,
      _CDL3LINESTRIKE,
      _CDL3OUTSIDE,
      _CDL3STARSINSOUTH,
      _CDL3WHITESOLDIERS,
      _CDLABANDONEDBABY,
      _CDLADVANCEBLOCK,
      _CDLBELTHOLD,
      _CDLBREAKAWAY,
      _CDLCLOSINGMARUBOZU,
      _CDLCONCEALBABYSWALL,
      _CDLCOUNTERATTACK,
      _CDLDARKCLOUDCOVER,
      _CDLDOJI,
      _CDLDOJISTAR,
      _CDLDRAGONFLYDOJI,
      _CDLENGULFING,
      _CDLEVENINGDOJISTAR,
      _CDLEVENINGSTAR,
      _CDLGAPSIDESIDEWHITE,
      _CDLGRAVESTONEDOJI,
      _CDLHAMMER,
      _CDLHANGINGMAN,
      _CDLHARAMI,
      _CDLHARAMICROSS,
      _CDLHIGHWAVE,
      _CDLHIKKAKE,
      _CDLHIKKAKEMOD,
      _CDLHOMINGPIGEON,
      _CDLIDENTICAL3CROWS,
      _CDLINNECK,
      _CDLINVERTEDHAMMER,
      _CDLKICKING,
      _CDLKICKINGBYLENGTH,
      _CDLLADDERBOTTOM,
      _CDLLONGLEGGEDDOJI,
      _CDLLONGLINE,
      _CDLMARUBOZU,
      _CDLMATCHINGLOW,
      _CDLMATHOLD,
      _CDLMORNINGDOJISTAR,
      _CDLMORNINGSTAR,
      _CDLONNECK,
      _CDLPIERCING,
      _CDLRICKSHAWMAN,
      _CDLRISEFALL3METHODS,
      _CDLSEPARATINGLINES,
      _CDLSHOOTINGSTAR,
      _CDLSHORTLINE,
      _CDLSPINNINGTOP,
      _CDLSTALLEDPATTERN,
      _CDLSTICKSANDWICH,
      _CDLTAKURI,
      _CDLTASUKIGAP,
      _CDLTHRUSTING,
      _CDLTRISTAR,
      _CDLUNIQUE3RIVER,
      _CDLUPSIDEGAP2CROWS,
      _CDLXSIDEGAP3METHODS,
      _CEIL,
      _CMO,
      _COLOR,
      _COMPARE,
      _COMPARE2,
      _CORREL,
      _COS,
      _COSH,
      _DEMA,
      _DIV,
      _DX,
      _EMA,
      _EXP,
      _FI,
      _FLOOR,
      _HT_DCPERIOD,
      _HT_DCPHASE,
      _HT_PHASOR,
      _HT_SINE,
      _HT_TRENDLINE,
      _HT_TRENDMODE,
      _KAMA,
      _LINEARREG,
      _LINEARREG_ANGLE,
      _LINEARREG_INTERCEPT,
      _LINEARREG_SLOPE,
      _LMS,
      _LN,
      _LOG10,
      _MACD,
      _MACDEXT,
      _MACDFIX,
      _MAMA,
      _MAVP,
      _MAX,
      _MAXINDEX,
      _MEDPRICE,
      _MFI,
      _MIDPOINT,
      _MIDPRICE,
      _MIN,
      _MININDEX,
      _MINMAX,
      _MINMAXINDEX,
      _MINUS_DI,
      _MINUS_DM,
      _MOM,
      _MULT,
      _NATR,
      _NORMAL,
      _OBV,
      _PP,
      _PLUS_DI,
      _PLUS_DM,
      _PPO,
      _REF,
      _ROC,
      _ROCP,
      _ROCR,
      _ROCR100,
      _RSI,
      _SAR,
      _SAREXT,
      _SIN,
      _SINH,
      _SINWAV,
      _SMA,
      _SQRT,
      _STDDEV,
      _STOCH,
      _STOCHF,
      _STOCHRSI,
      _SUB,
      _SUM,
      _SYMBOL,
      _SZ,
      _T3,
      _TAN,
      _TANH,
      _TEMA,
      _THERM,
      _TRANGE,
      _TRIMA,
      _TRIX,
      _TSF,
      _TYPPRICE,
      _ULTOSC,
      _VAR,
      _VFI,
      _VIDYA,
      _WCLPRICE,
      _WILLR,
      _WMA
    };

    SCIndicator ();
    int calculate (QStringList &l, QHash<QString, PlotLine *> &tlines, BarData *data);

  private:
    QStringList indicatorList;
    QStringList maList;
    QStringList opList;
};

#endif
