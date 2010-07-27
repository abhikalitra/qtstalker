/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "FunctionCANDLES.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionCANDLES::FunctionCANDLES ()
{
  _methodList << "NONE";
  _methodList << "2CROWS";
  _methodList << "3BLACKCROWS";
  _methodList << "3INSIDE";
  _methodList << "3LINESTRIKE";
  _methodList << "3OUTSIDE";
  _methodList << "3STARSINSOUTH";
  _methodList << "3WHITESOLDIERS";
  _methodList << "ABANDONEDBABY";
  _methodList << "ADVANCEBLOCK";
  _methodList << "BELTHOLD";
  _methodList << "BREAKAWAY";
  _methodList << "CLOSINGMARUBOZU";
  _methodList << "CONCEALBABYSWALL";
  _methodList << "COUNTERATTACK";
  _methodList << "DARKCLOUDCOVER";
  _methodList << "DOJI";
  _methodList << "DOJISTAR";
  _methodList << "DRAGONFLYDOJI";
  _methodList << "ENGULFING";
  _methodList << "EVENINGDOJISTAR";
  _methodList << "EVENINGSTAR";
  _methodList << "GAPSIDESIDEWHITE";
  _methodList << "GRAVESTONEDOJI";
  _methodList << "HAMMER";
  _methodList << "HANGINGMAN";
  _methodList << "HARAMI";
  _methodList << "HARAMICROSS";
  _methodList << "HIGHWAVE";
  _methodList << "HIKKAKE";
  _methodList << "HIKKAKEMOD";
  _methodList << "HOMINGPIGEON";
  _methodList << "IDENTICAL3CROWS";
  _methodList << "INNECK";
  _methodList << "INVERTEDHAMMER";
  _methodList << "KICKING";
  _methodList << "KICKINGBYLENGTH";
  _methodList << "LADDERBOTTOM";
  _methodList << "LONGLEGGEDDOJI";
  _methodList << "LONGLINE";
  _methodList << "MARUBOZU";
  _methodList << "MATCHINGLOW";
  _methodList << "MATHOLD";
  _methodList << "MORNINGDOJISTAR";
  _methodList << "MORNINGSTAR";
  _methodList << "ONNECK";
  _methodList << "PIERCING";
  _methodList << "RICKSHAWMAN";
  _methodList << "RISEFALL3METHODS";
  _methodList << "SEPARATINGLINES";
  _methodList << "SHOOTINGSTAR";
  _methodList << "SHORTLINE";
  _methodList << "SPINNINGTOP";
  _methodList << "STALLEDPATTERN";
  _methodList << "STICKSANDWICH";
  _methodList << "TAKURI";
  _methodList << "TASUKIGAP";
  _methodList << "THRUSTING";
  _methodList << "TRISTAR";
  _methodList << "UNIQUE3RIVER";
  _methodList << "UPSIDEGAP2CROWS";
  _methodList << "XSIDEGAP3METHODS";
}

int FunctionCANDLES::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,*
  //     0       1      2        3

  if (set.count() < 4)
  {
    qDebug() << "FunctionCANDLES::script: invalid parm count" << set.count();
    return 1;
  }

  int rc = 1;
  int method = _methodList.indexOf(set[3]);
  switch (method)
  {
    case _NONE:
      rc = scriptCandles(set, ind, data);
      break;
    default:
      rc = scriptMethod(set, ind, data);
      break;
  }

  return rc;
}

int FunctionCANDLES::scriptCandles (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,<NAME>
  //    0        1       2       3       4
  
  if (set.count() != 5)
  {
    qDebug() << "FunctionCANDLES::scriptCandles: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionCANDLES::scriptCandles: duplicate name" << set[4];
    return 1;
  }

  PlotLine *line = candles(data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

int FunctionCANDLES::scriptMethod (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,<NAME>,<PENETRATION>
  //    0        1       2       3       4         5 

  if (set.count() != 6)
  {
    qDebug() << "FunctionCANDLES::scriptMethod: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << "FunctionCANDLES::scriptMethod: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionCANDLES::scriptMethod: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  double pen = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionCANDLES::scriptMethod: invalid penetration" << set[5];
    return 1;
  }

  PlotLine *line = getMethod(data, method, pen);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

PlotLine * FunctionCANDLES::candles (BarData &data)
{
  if (! data.count())
    return 0;
  
  QString s = "Candle";
  PlotLine *line = new PlotLine;
  line->setType(s);

  int loop;
  int size = data.count();
  for (loop = 0; loop < size; loop++)
  {
    PlotLineBar *bar = new PlotLineBar;
    Bar tbar = data.getBar(loop);
    bar->setData(0, tbar.getOpen());
    bar->setData(1, tbar.getHigh());
    bar->setData(2, tbar.getLow());
    bar->setData(3, tbar.getClose());
    line->setData(loop, bar);
  }

  return line;
}

PlotLine * FunctionCANDLES::getMethod (BarData &data, int method, double pen)
{
  if (data.count() < 1)
    return 0;

  int size = data.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real open[size];
  TA_Integer out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop;
  for (loop = 0; loop < size; loop++)
  {
    Bar bar = data.getBar(loop);
    open[loop] = (TA_Real) bar.getOpen();
    high[loop] = (TA_Real) bar.getHigh();
    low[loop] = (TA_Real) bar.getLow();
    close[loop] = (TA_Real) bar.getClose();
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _ABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _DARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _EVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _EVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _MORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _2CROWS:
      rc = TA_CDL2CROWS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _3BLACKCROWS:
      rc = TA_CDL3BLACKCROWS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _3INSIDE:
      rc = TA_CDL3INSIDE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _3LINESTRIKE:
      rc = TA_CDL3LINESTRIKE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _3OUTSIDE:
      rc = TA_CDL3OUTSIDE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _3STARSINSOUTH:
      rc = TA_CDL3STARSINSOUTH(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _3WHITESOLDIERS:
      rc = TA_CDL3WHITESOLDIERS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _ADVANCEBLOCK:
      rc = TA_CDLADVANCEBLOCK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _BELTHOLD:
      rc = TA_CDLBELTHOLD(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _BREAKAWAY:
      rc = TA_CDLBREAKAWAY(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _CLOSINGMARUBOZU:
      rc = TA_CDLCLOSINGMARUBOZU(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _CONCEALBABYSWALL:
      rc = TA_CDLCONCEALBABYSWALL(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _COUNTERATTACK:
      rc = TA_CDLCOUNTERATTACK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _DOJI:
      rc = TA_CDLDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _DOJISTAR:
      rc = TA_CDLDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _DRAGONFLYDOJI:
      rc = TA_CDLDRAGONFLYDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _ENGULFING:
      rc = TA_CDLENGULFING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _GAPSIDESIDEWHITE:
      rc = TA_CDLGAPSIDESIDEWHITE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _GRAVESTONEDOJI:
      rc = TA_CDLGRAVESTONEDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HAMMER:
      rc = TA_CDLHAMMER(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HANGINGMAN:
      rc = TA_CDLHANGINGMAN(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HARAMI:
      rc = TA_CDLHARAMI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HARAMICROSS:
      rc = TA_CDLHARAMICROSS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HIGHWAVE:
      rc = TA_CDLHIGHWAVE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HIKKAKE:
      rc = TA_CDLHIKKAKE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HIKKAKEMOD:
      rc = TA_CDLHIKKAKEMOD(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _HOMINGPIGEON:
      rc = TA_CDLHOMINGPIGEON(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _IDENTICAL3CROWS:
      rc = TA_CDLIDENTICAL3CROWS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _INNECK:
      rc = TA_CDLINNECK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _INVERTEDHAMMER:
      rc = TA_CDLINVERTEDHAMMER(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _KICKING:
      rc = TA_CDLKICKING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _KICKINGBYLENGTH:
      rc = TA_CDLKICKINGBYLENGTH(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _LADDERBOTTOM:
      rc = TA_CDLLADDERBOTTOM(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _LONGLEGGEDDOJI:
      rc = TA_CDLLONGLEGGEDDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _LONGLINE:
      rc = TA_CDLLONGLINE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _MARUBOZU:
      rc = TA_CDLMARUBOZU(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _MATCHINGLOW:
      rc = TA_CDLMATCHINGLOW(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _ONNECK:
      rc = TA_CDLONNECK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _PIERCING:
      rc = TA_CDLPIERCING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _RICKSHAWMAN:
      rc = TA_CDLRICKSHAWMAN(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _RISEFALL3METHODS:
      rc = TA_CDLRISEFALL3METHODS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _SEPARATINGLINES:
      rc = TA_CDLSEPARATINGLINES(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _SHOOTINGSTAR:
      rc = TA_CDLSHOOTINGSTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _SHORTLINE:
      rc = TA_CDLSHORTLINE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _SPINNINGTOP:
      rc = TA_CDLSPINNINGTOP(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _STALLEDPATTERN:
      rc = TA_CDLSTALLEDPATTERN(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _STICKSANDWICH:
      rc = TA_CDLSTICKSANDWICH(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _TAKURI:
      rc = TA_CDLTAKURI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _TASUKIGAP:
      rc = TA_CDLTASUKIGAP(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _THRUSTING:
      rc = TA_CDLTHRUSTING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _TRISTAR:
      rc = TA_CDLTRISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _UNIQUE3RIVER:
      rc = TA_CDLUNIQUE3RIVER(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _UPSIDEGAP2CROWS:
      rc = TA_CDLUPSIDEGAP2CROWS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case _XSIDEGAP3METHODS:
      rc = TA_CDLXSIDEGAP3METHODS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionCANDLES::method: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;

  int dataLoop = data.count() - 1;
  int outLoop = outNb - 1;
  while (dataLoop > -1 && outLoop > -1)
  {
    line->setData(dataLoop, out[outLoop]);
    dataLoop--;
    outLoop--;
  }

  return line;
}

QStringList & FunctionCANDLES::list ()
{
  return _methodList;
}

