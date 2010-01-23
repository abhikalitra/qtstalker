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
#include "FI.h"
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
#include "LMS.h"
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
#include "PP.h"
#include "PLUS_DI.h"
#include "PLUS_DM.h"
#include "PPO.h"
#include "REF.h"
#include "ROC.h"
#include "RSI.h"
#include "SAR.h"
#include "SIN.h"
#include "SINWAV.h"
#include "SMA.h"
#include "SQRT.h"
#include "STDDEV.h"
#include "STOCH.h"
#include "SUB.h"
#include "SUM.h"
#include "SYMBOL.h"
#include "SZ.h"
#include "T3.h"
#include "TAN.h"
#include "TEMA.h"
#include "THERM.h"
#include "TRANGE.h"
#include "TRIMA.h"
#include "TRIX.h"
#include "TSF.h"
#include "TYPPRICE.h"
#include "ULTOSC.h"
#include "VAR.h"
#include "VFI.h"
#include "VIDYA.h"
#include "WCLPRICE.h"
#include "WILLR.h"
#include "WMA.h"

#include <QByteArray>
#include <QtDebug>


SCIndicator::SCIndicator ()
{
  maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA" << "MAMA" << "T3";

  opList << "=" << "<" << "<=" << ">" << ">=" << "AND" << "OR";

  indicatorList << "ACOS";
  indicatorList << "AD";
  indicatorList << "ADD";
  indicatorList << "ADOSC";
  indicatorList << "ADX";
  indicatorList << "ADXR";
  indicatorList << "APO";
  indicatorList << "AROON";
  indicatorList << "AROONOSC";
  indicatorList << "ASIN";
  indicatorList << "ATAN";
  indicatorList << "ATR";
  indicatorList << "AVGPRICE";
  indicatorList << "BARS";
  indicatorList << "BBANDS";
  indicatorList << "BETA";
  indicatorList << "BOP";
  indicatorList << "CANDLES";
  indicatorList << "CCI";
  indicatorList << "CDL2CROWS";
  indicatorList << "CDL3BLACKCROWS";
  indicatorList << "CDL3INSIDE";
  indicatorList << "CDL3LINESTRIKE";
  indicatorList << "CDL3OUTSIDE";
  indicatorList << "CDL3STARSINSOUTH";
  indicatorList << "CDL3WHITESOLDIERS";
  indicatorList << "CDLABANDONEDBABY";
  indicatorList << "CDLADVANCEBLOCK";
  indicatorList << "CDLBELTHOLD";
  indicatorList << "CDLBREAKAWAY";
  indicatorList << "CDLCLOSINGMARUBOZU";
  indicatorList << "CDLCONCEALBABYSWALL";
  indicatorList << "CDLCOUNTERATTACK";
  indicatorList << "CDLDARKCLOUDCOVER";
  indicatorList << "CDLDOJI";
  indicatorList << "CDLDOJISTAR";
  indicatorList << "CDLDRAGONFLYDOJI";
  indicatorList << "CDLENGULFING";
  indicatorList << "CDLEVENINGDOJISTAR";
  indicatorList << "CDLEVENINGSTAR";
  indicatorList << "CDLGAPSIDESIDEWHITE";
  indicatorList << "CDLGRAVESTONEDOJI";
  indicatorList << "CDLHAMMER";
  indicatorList << "CDLHANGINGMAN";
  indicatorList << "CDLHARAMI";
  indicatorList << "CDLHARAMICROSS";
  indicatorList << "CDLHIGHWAVE";
  indicatorList << "CDLHIKKAKE";
  indicatorList << "CDLHIKKAKEMOD";
  indicatorList << "CDLHOMINGPIGEON";
  indicatorList << "CDLIDENTICAL3CROWS";
  indicatorList << "CDLINNECK";
  indicatorList << "CDLINVERTEDHAMMER";
  indicatorList << "CDLKICKING";
  indicatorList << "CDLKICKINGBYLENGTH";
  indicatorList << "CDLLADDERBOTTOM";
  indicatorList << "CDLLONGLEGGEDDOJI";
  indicatorList << "CDLLONGLINE";
  indicatorList << "CDLMARUBOZU";
  indicatorList << "CDLMATCHINGLOW";
  indicatorList << "CDLMATHOLD";
  indicatorList << "CDLMORNINGDOJISTAR";
  indicatorList << "CDLMORNINGSTAR";
  indicatorList << "CDLONNECK";
  indicatorList << "CDLPIERCING";
  indicatorList << "CDLRICKSHAWMAN";
  indicatorList << "CDLRISEFALL3METHODS";
  indicatorList << "CDLSEPARATINGLINES";
  indicatorList << "CDLSHOOTINGSTAR";
  indicatorList << "CDLSHORTLINE";
  indicatorList << "CDLSPINNINGTOP";
  indicatorList << "CDLSTALLEDPATTERN";
  indicatorList << "CDLSTICKSANDWICH";
  indicatorList << "CDLTAKURI";
  indicatorList << "CDLTASUKIGAP";
  indicatorList << "CDLTHRUSTING";
  indicatorList << "CDLTRISTAR";
  indicatorList << "CDLUNIQUE3RIVER";
  indicatorList << "CDLUPSIDEGAP2CROWS";
  indicatorList << "CDLXSIDEGAP3METHODS";
  indicatorList << "CEIL";
  indicatorList << "CMO";
  indicatorList << "COLOR";
  indicatorList << "COMPARE";
  indicatorList << "COMPARE2";
  indicatorList << "CORREL";
  indicatorList << "COS";
  indicatorList << "COSH";
  indicatorList << "DEMA";
  indicatorList << "DIV";
  indicatorList << "DX";
  indicatorList << "EMA";
  indicatorList << "EXP";
  indicatorList << "FI";
  indicatorList << "FLOOR";
  indicatorList << "HT_DCPERIOD";
  indicatorList << "HT_DCPHASE";
  indicatorList << "HT_PHASOR";
  indicatorList << "HT_SINE";
  indicatorList << "HT_TRENDLINE";
  indicatorList << "HT_TRENDMODE";
  indicatorList << "KAMA";
  indicatorList << "LINEARREG";
  indicatorList << "LINEARREG_ANGLE";
  indicatorList << "LINEARREG_INTERCEPT";
  indicatorList << "LINEARREG_SLOPE";
  indicatorList << "LMS";
  indicatorList << "LN";
  indicatorList << "LOG10";
  indicatorList << "MACD";
  indicatorList << "MACDEXT";
  indicatorList << "MACDFIX";
  indicatorList << "MAMA";
  indicatorList << "MAVP";
  indicatorList << "MAX";
  indicatorList << "MAXINDEX";
  indicatorList << "MEDPRICE";
  indicatorList << "MFI";
  indicatorList << "MIDPOINT";
  indicatorList << "MIDPRICE";
  indicatorList << "MIN";
  indicatorList << "MININDEX";
  indicatorList << "MINMAX";
  indicatorList << "MINMAXINDEX";
  indicatorList << "MINUS_DI";
  indicatorList << "MINUS_DM";
  indicatorList << "MOM";
  indicatorList << "MULT";
  indicatorList << "NATR";
  indicatorList << "NORMAL";
  indicatorList << "OBV";
  indicatorList << "PP";
  indicatorList << "PLUS_DI";
  indicatorList << "PLUS_DM";
  indicatorList << "PPO";
  indicatorList << "REF";
  indicatorList << "ROC";
  indicatorList << "ROCP";
  indicatorList << "ROCR";
  indicatorList << "ROCR100";
  indicatorList << "RSI";
  indicatorList << "SAR";
  indicatorList << "SAREXT";
  indicatorList << "SIN";
  indicatorList << "SINH";
  indicatorList << "SINWAV";
  indicatorList << "SMA";
  indicatorList << "SQRT";
  indicatorList << "STDDEV";
  indicatorList << "STOCH";
  indicatorList << "STOCHF";
  indicatorList << "STOCHRSI";
  indicatorList << "SUB";
  indicatorList << "SUM";
  indicatorList << "SYMBOL";
  indicatorList << "SZ";
  indicatorList << "T3";
  indicatorList << "TAN";
  indicatorList << "TANH";
  indicatorList << "TEMA";
  indicatorList << "THERM";
  indicatorList << "TRANGE";
  indicatorList << "TRIMA";
  indicatorList << "TRIX";
  indicatorList << "TSF";
  indicatorList << "TYPPRICE";
  indicatorList << "ULTOSC";
  indicatorList << "VAR";
  indicatorList << "VFI";
  indicatorList << "VIDYA";
  indicatorList << "WCLPRICE";
  indicatorList << "WILLR";
  indicatorList << "WMA";
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
    case _FI:
    {
      FI ind;
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
    case _LMS:
    {
      LMS ind;
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
    case _PP:
    {
      PP ind;
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
    case _SINWAV:
    {
      SINWAV ind;
      rc = ind.calculate(l, tlines, data);
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
    case _SZ:
    {
      SZ ind;
      rc = ind.calculate(l, tlines, data);
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
    case _THERM:
    {
      THERM ind;
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
    case _VFI:
    {
      VFI ind;
      rc = ind.calculate(l, tlines, data);
      break;
    }
    case _VIDYA:
    {
      VIDYA ind;
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

