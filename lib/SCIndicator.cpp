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

#include "SCIndicator.h"

#include "ACOS.h"
#include "AD.h"
#include "ADD.h"
#include "ADX.h"
#include "APO.h"
#include "AROON.h"
#include "ASIN.h"
#include "ATAN.h"
#include "AVGPRICE.h"
#include "BARS.h"
#include "BBANDS.h"
#include "BETA.h"
#include "BOP.h"
#include "CANDLES.h"
#include "CCI.h"
#include "CDL.h"
#include "CEIL.h"
#include "CMO.h"
#include "COLOR.h"
#include "COMPARE.h"
#include "CORREL.h"
#include "COS.h"
#include "DEMA.h"
#include "DIV.h"
#include "DX.h"
#include "EMA.h"
#include "EXP.h"
#include "FLOOR.h"
#include "HT_DCPERIOD.h"
#include "HT_DCPHASE.h"
#include "HT_PHASOR.h"
#include "HT_SINE.h"
#include "HT_TRENDLINE.h"
#include "HT_TRENDMODE.h"
#include "KAMA.h"
#include "LINEARREG.h"
#include "LINEARREG_ANGLE.h"
#include "LINEARREG_INTERCEPT.h"
#include "LINEARREG_SLOPE.h"
#include "LN.h"
#include "LOG10.h"
#include "MACD.h"
#include "MAMA.h"
#include "MAVP.h"
#include "MAX.h"
#include "MAXINDEX.h"
#include "MEDPRICE.h"
#include "MFI.h"
#include "MIDPOINT.h"
#include "MIDPRICE.h"
#include "MIN.h"
#include "MININDEX.h"
#include "MINMAX.h"
#include "MINMAXINDEX.h"
#include "MINUS_DI.h"
#include "MINUS_DM.h"
#include "MOM.h"
#include "MULT.h"
#include "NATR.h"
#include "NORMAL.h"
#include "OBV.h"
#include "PLUS_DI.h"
#include "PLUS_DM.h"
#include "PPO.h"
#include "REF.h"
#include "ROC.h"
#include "RSI.h"
#include "SAR.h"
#include "SIN.h"
#include "SMA.h"
#include "SQRT.h"
#include "STDDEV.h"
#include "STOCH.h"
#include "SUB.h"
#include "SUM.h"
#include "SYMBOL.h"
#include "T3.h"
#include "TAN.h"
#include "TEMA.h"
#include "TRANGE.h"
#include "TRIMA.h"
#include "TRIX.h"
#include "TSF.h"
#include "TYPPRICE.h"
#include "ULTOSC.h"
#include "VAR.h"
#include "WCLPRICE.h"
#include "WILLR.h"
#include "WMA.h"

#include <QByteArray>
#include <QtDebug>


SCIndicator::SCIndicator ()
{
  maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA" << "MAMA" << "T3";

  opList << "=" << "<" << "<=" << ">" << ">=" << "AND" << "OR";

  indicatorList << "ACOS" << "AD" << "ADD" << "ADOSC" << "ADX" << "ADXR" << "APO" << "AROON" << "AROONOSC" << "ASIN";
  indicatorList << "ATAN" << "ATR" << "AVGPRICE" << "BARS" << "BBANDS" << "BETA" << "BOP" << "CANDLES" << "CCI";
  indicatorList << "CDL2CROWS" << "CDL3BLACKCROWS" << "CDL3INSIDE" << "CDL3LINESTRIKE" << "CDL3OUTSIDE";
  indicatorList << "CDL3STARSINSOUTH" << "CDL3WHITESOLDIERS" << "CDLABANDONEDBABY" << "CDLADVANCEBLOCK" << "CDLBELTHOLD";
  indicatorList << "CDLBREAKAWAY" << "CDLCLOSINGMARUBOZU" << "CDLCONCEALBABYSWALL" << "CDLCOUNTERATTACK" << "CDLDARKCLOUDCOVER";
  indicatorList << "CDLDOJI" << "CDLDOJISTAR" << "CDLDRAGONFLYDOJI" << "CDLENGULFING" << "CDLEVENINGDOJISTAR" << "CDLEVENINGSTAR";
  indicatorList << "CDLGAPSIDESIDEWHITE" << "CDLGRAVESTONEDOJI" << "CDLHAMMER" << "CDLHANGINGMAN" << "CDLHARAMI" << "CDLHARAMICROSS";
  indicatorList << "CDLHIGHWAVE" << "CDLHIKKAKE" << "CDLHIKKAKEMOD" << "CDLHOMINGPIGEON" << "CDLIDENTICAL3CROWS" << "CDLINNECK";
  indicatorList << "CDLINVERTEDHAMMER" << "CDLKICKING" << "CDLKICKINGBYLENGTH" << "CDLLADDERBOTTOM" << "CDLLONGLEGGEDDOJI";
  indicatorList << "CDLLONGLINE" << "CDLMARUBOZU" << "CDLMATCHINGLOW" << "CDLMATHOLD" << "CDLMORNINGDOJISTAR";
  indicatorList << "CDLMORNINGSTAR" << "CDLONNECK" << "CDLPIERCING" << "CDLRICKSHAWMAN" << "CDLRISEFALL3METHODS";
  indicatorList << "CDLSEPARATINGLINES" << "CDLSHOOTINGSTAR" << "CDLSHORTLINE" << "CDLSPINNINGTOP" << "CDLSTALLEDPATTERN";
  indicatorList << "CDLSTICKSANDWICH" << "CDLTAKURI" << "CDLTASUKIGAP" << "CDLTHRUSTING" << "CDLTRISTAR" << "CDLUNIQUE3RIVER";
  indicatorList << "CDLUPSIDEGAP2CROWS" << "CDLXSIDEGAP3METHODS" << "CEIL" << "CMO" << "COLOR" << "COMPARE" << "COMPARE2";
  indicatorList << "CORREL" << "COS" << "COSH" << "DEMA" << "DIV" << "DX" << "EMA" << "EXP" << "FLOOR" << "HT_DCPERIOD";
  indicatorList << "HT_DCPHASE" << "HT_PHASOR" << "HT_SINE" << "HT_TRENDLINE" << "HT_TRENDMODE" << "KAMA" << "LINEARREG";
  indicatorList << "LINEARREG_ANGLE" << "LINEARREG_INTERCEPT" << "LINEARREG_SLOPE" << "LN" << "LOG10" << "MACD" << "MACDEXT";
  indicatorList << "MACDFIX" << "MAMA" << "MAVP" << "MAX" << "MAXINDEX" << "MEDPRICE" << "MFI" << "MIDPOINT" << "MIDPRICE";
  indicatorList << "MIN" << "MININDEX" << "MINMAX" << "MINMAXINDEX" << "MINUS_DI" << "MINUS_DM" << "MOM" << "MULT" << "NATR";
  indicatorList << "NORMAL" << "OBV" << "PLUS_DI" << "PLUS_DM" << "PPO" << "REF" << "ROC" << "ROCP" << "ROCR" << "ROCR100";
  indicatorList << "RSI" << "SAR" << "SAREXT" << "SIN" << "SINH" << "SMA" << "SQRT" << "STDDEV" << "STOCH" << "STOCHF";
  indicatorList << "STOCHRSI" << "SUB" << "SUM" << "SYMBOL" << "T3" << "TAN" << "TANH" << "TEMA" << "TRANGE" << "TRIMA";
  indicatorList << "TRIX" << "TSF" << "TYPPRICE" << "ULTOSC" << "VAR" << "WCLPRICE" << "WILLR" << "WMA";
}

int SCIndicator::calculate (QStringList &l, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  int rc = FALSE;
  int i = indicatorList.indexOf(l[1]);
  if (i < 0)
    return 1;

  switch ((IndicatorName) i)
  {
    case _ACOS:
    {
      ACOS ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _AD:
    {
      AD ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _ADOSC:
    {
      AD ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _ADD:
    {
      ADD ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _ADX:
    {
      ADX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _ADXR:
    {
      ADX ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _APO:
    {
      APO ind;
      rc = ind.calculate(l, tlines, maList, data);
      break;
    }
    case _AROON:
    {
      AROON ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _AROONOSC:
    {
      AROON ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _ASIN:
    {
      ASIN ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _ATAN:
    {
      ATAN ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _ATR:
    {
      TRANGE ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _AVGPRICE:
    {
      AVGPRICE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _BARS:
    {
      BARS ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _BBANDS:
    {
      BBANDS ind;
      rc = ind.calculate(l, tlines, maList, data);
      break;
    }
    case _BETA:
    {
      BETA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _BOP:
    {
      BOP ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _CANDLES:
    {
      CANDLES ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _CCI:
    {
      CCI ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _CDL2CROWS:
    case _CDL3BLACKCROWS:
    case _CDL3INSIDE:
    case _CDL3LINESTRIKE:
    case _CDL3OUTSIDE:
    case _CDL3STARSINSOUTH:
    case _CDL3WHITESOLDIERS:
    case _CDLABANDONEDBABY:
    case _CDLADVANCEBLOCK:
    case _CDLBELTHOLD:
    case _CDLBREAKAWAY:
    case _CDLCLOSINGMARUBOZU:
    case _CDLCONCEALBABYSWALL:
    case _CDLCOUNTERATTACK:
    case _CDLDARKCLOUDCOVER:
    case _CDLDOJI:
    case _CDLDOJISTAR:
    case _CDLDRAGONFLYDOJI:
    case _CDLENGULFING:
    case _CDLEVENINGDOJISTAR:
    case _CDLEVENINGSTAR:
    case _CDLGAPSIDESIDEWHITE:
    case _CDLGRAVESTONEDOJI:
    case _CDLHAMMER:
    case _CDLHANGINGMAN:
    case _CDLHARAMI:
    case _CDLHARAMICROSS:
    case _CDLHIGHWAVE:
    case _CDLHIKKAKE:
    case _CDLHIKKAKEMOD:
    case _CDLHOMINGPIGEON:
    case _CDLIDENTICAL3CROWS:
    case _CDLINNECK:
    case _CDLINVERTEDHAMMER:
    case _CDLKICKING:
    case _CDLKICKINGBYLENGTH:
    case _CDLLADDERBOTTOM:
    case _CDLLONGLEGGEDDOJI:
    case _CDLLONGLINE:
    case _CDLMARUBOZU:
    case _CDLMATCHINGLOW:
    case _CDLMATHOLD:
    case _CDLMORNINGDOJISTAR:
    case _CDLMORNINGSTAR:
    case _CDLONNECK:
    case _CDLPIERCING:
    case _CDLRICKSHAWMAN:
    case _CDLRISEFALL3METHODS:
    case _CDLSEPARATINGLINES:
    case _CDLSHOOTINGSTAR:
    case _CDLSHORTLINE:
    case _CDLSPINNINGTOP:
    case _CDLSTALLEDPATTERN:
    case _CDLSTICKSANDWICH:
    case _CDLTAKURI:
    case _CDLTASUKIGAP:
    case _CDLTHRUSTING:
    case _CDLTRISTAR:
    case _CDLUNIQUE3RIVER:
    case _CDLUPSIDEGAP2CROWS:
    case _CDLXSIDEGAP3METHODS:
    {
      CDL ind;
      rc = ind.calculate(l, tlines, i, data);
      break;
    }
    case _CEIL:
    {
      CEIL ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _CMO:
    {
      CMO ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _COLOR:
    {
      COLOR ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _COMPARE:
    {
      COMPARE ind;
      rc = ind.calculate(l, tlines, opList, data);
      break;
    }
    case _COMPARE2:
    {
      COMPARE ind;
      rc = ind.calculate2(l, tlines, opList, data);
      break;
    }
    case _CORREL:
    {
      CORREL ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _COS:
    {
      COS ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _COSH:
    {
      COS ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _DEMA:
    {
      DEMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _DIV:
    {
      DIV ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _DX:
    {
      DX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _EMA:
    {
      EMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _EXP:
    {
      EXP ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _FLOOR:
    {
      FLOOR ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _HT_DCPERIOD:
    {
      HT_DCPERIOD ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _HT_DCPHASE:
    {
      HT_DCPHASE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _HT_PHASOR:
    {
      HT_PHASOR ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _HT_SINE:
    {
      HT_SINE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _HT_TRENDLINE:
    {
      HT_TRENDLINE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _HT_TRENDMODE:
    {
      HT_TRENDMODE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _KAMA:
    {
      KAMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _LINEARREG:
    {
      LINEARREG ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _LINEARREG_ANGLE:
    {
      LINEARREG_ANGLE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _LINEARREG_INTERCEPT:
    {
      LINEARREG_INTERCEPT ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _LINEARREG_SLOPE:
    {
      LINEARREG_SLOPE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _LN:
    {
      LN ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _LOG10:
    {
      LOG10 ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MACD:
    {
      MACD ind;
      rc = ind.calculate1(l, tlines, data);
      break;
    }
    case _MACDEXT:
    {
      MACD ind;
      rc = ind.calculate2(l, tlines, maList, data);
      break;
    }
    case _MACDFIX:
    {
      MACD ind;
      rc = ind.calculate3(l, tlines, data);
      break;
    }
    case _MAMA:
    {
      MAMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MAVP:
    {
      MAVP ind;
      rc = ind.calculate(l, tlines, maList, data);
      break;
    }
    case _MAX:
    {
      MAX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MAXINDEX:
    {
      MAXINDEX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MEDPRICE:
    {
      MEDPRICE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MFI:
    {
      MFI ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MIDPOINT:
    {
      MIDPOINT ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MIDPRICE:
    {
      MIDPRICE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MIN:
    {
      MIN ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MININDEX:
    {
      MININDEX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MINMAX:
    {
      MINMAX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MINMAXINDEX:
    {
      MINMAXINDEX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MINUS_DI:
    {
      MINUS_DI ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MINUS_DM:
    {
      MINUS_DM ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MOM:
    {
      MOM ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _MULT:
    {
      MULT ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _NATR:
    {
      NATR ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _NORMAL:
    {
      NORMAL ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _OBV:
    {
      OBV ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _PLUS_DI:
    {
      PLUS_DI ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _PLUS_DM:
    {
      PLUS_DM ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _PPO:
    {
      PPO ind;
      rc = ind.calculate(l, tlines, maList, data);
      break;
    }
    case _REF:
    {
      REF ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _ROC:
    case _ROCP:
    case _ROCR:
    case _ROCR100:
    {
      ROC ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _RSI:
    {
      RSI ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _SAR:
    {
      SAR ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _SAREXT:
    {
      SAR ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _SIN:
    {
      SIN ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _SINH:
    {
      SIN ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _SMA:
    {
      SMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _SQRT:
    {
      SQRT ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _STDDEV:
    {
      STDDEV ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _STOCH:
    {
      STOCH ind;
      rc = ind.calculate(l, tlines, maList, data);
      break;
    }
    case _STOCHF:
    {
      STOCH ind;
      rc = ind.calculate2(l, tlines, maList, data);
      break;
    }
    case _STOCHRSI:
    {
      STOCH ind;
      rc = ind.calculate3(l, tlines, maList);
      break;
    }
    case _SUB:
    {
      SUB ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _SUM:
    {
      SUM ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _SYMBOL:
    {
      SYMBOL ind;
      rc = ind.calculate(l, tlines);
      break;
    }
    case _T3:
    {
      T3 ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _TAN:
    {
      TAN ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _TANH:
    {
      TAN ind;
      rc = ind.calculate2(l, tlines, data);
      break;
    }
    case _TEMA:
    {
      TEMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _TRANGE:
    {
      TRANGE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _TRIMA:
    {
      TRIMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _TRIX:
    {
      TRIX ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _TSF:
    {
      TSF ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _TYPPRICE:
    {
      TYPPRICE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _ULTOSC:
    {
      ULTOSC ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _VAR:
    {
      VAR ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _WCLPRICE:
    {
      WCLPRICE ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _WILLR:
    {
      WILLR ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _WMA:
    {
      WMA ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    default:
      break;
  }

  return rc;
}

