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

#include "CANDLES.h"
#include "FunctionMA.h"
#include "CANDLESDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  _indicator = "CANDLES";

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

int CANDLES::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  Curve *line = candles(data);
  if (! line)
    return 1;

  QString s;
  settings.getData(_Color, s);
  QColor c(s);
  line->setAllColor(c);

  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);
  if (method != _NONE)
  {
    double pen = settings.getDouble(_Penetration);

    Curve *line2 = getMethod(method, pen, data);
    if (line2)
    {
      settings.getData(_MethodColor, s);
      QColor color(s);
      
      int loop = 0;
      for (; loop < line2->count(); loop++)
      {
        CurveBar *bar2 = line2->bar(loop);
        if (bar2->data() > 0)
        {
          CurveBar *bar = line->bar(loop);
          bar->setColor(color);
        }
      }
      
      delete line2;
    }
  }

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(0);
  ind.setLine(s, line);

  int period = settings.getInt(_MAPeriod);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(_MAType, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(_MAPlot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(_MAColor, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(_MALabel, s);
      ma->setLabel(s);
      
      ind.setLine(s, ma);

      ma->setZ(1);
    }
  }

  period = settings.getInt(_MA2Period);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(_MA2Type, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(_MA2Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(_MA2Color, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(_MA2Label, s);
      ma->setLabel(s);
      
      ind.setLine(s, ma);
      
      ma->setZ(2);
    }
  }

  period = settings.getInt(_MA3Period);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(_MA3Type, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(_MA3Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(_MA3Color, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(_MA3Label, s);
      ma->setLabel(s);
      
      ind.setLine(s, ma);
      
      ma->setZ(3);
    }
  }

  return 0;
}

int CANDLES::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,*
  //     0       1      2        3

  if (set.count() < 4)
  {
    qDebug() << "CANDLES::getCUS: invalid parm count" << set.count();
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

IndicatorPluginDialog * CANDLES::dialog (Indicator &i)
{
  return new CANDLESDialog(i);
}

void CANDLES::defaults (Indicator &i)
{
  Setting set;
  set.setData(CANDLES::_Penetration, 0.3);
  set.setData(CANDLES::_Method, "NONE");
  set.setData(CANDLES::_MethodColor, "cyan");
  set.setData(CANDLES::_Color, "green");
  set.setData(CANDLES::_Label, _indicator);
  set.setData(CANDLES::_MAColor, "red");
  set.setData(CANDLES::_MA2Color, "yellow");
  set.setData(CANDLES::_MA3Color, "blue");
  set.setData(CANDLES::_MAPlot, "Line");
  set.setData(CANDLES::_MA2Plot, "Line");
  set.setData(CANDLES::_MA3Plot, "Line");
  set.setData(CANDLES::_MALabel, "MA1");
  set.setData(CANDLES::_MA2Label, "MA2");
  set.setData(CANDLES::_MA3Label, "MA3");
  set.setData(CANDLES::_MAPeriod, 20);
  set.setData(CANDLES::_MA2Period, 50);
  set.setData(CANDLES::_MA3Period, 200);
  set.setData(CANDLES::_MAType, "SMA");
  set.setData(CANDLES::_MA2Type, "SMA");
  set.setData(CANDLES::_MA3Type, "SMA");
  i.setSettings(set);
}

void CANDLES::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_Label, s);
  l.append(s);

  settings.getData(_MALabel, s);
  l.append(s);

  settings.getData(_MA2Label, s);
  l.append(s);

  settings.getData(_MA3Label, s);
  l.append(s);
}

int CANDLES::scriptCandles (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,<NAME>
  //    0        1       2       3       4

  if (set.count() != 5)
  {
    qDebug() << "CANDLES::scriptCandles: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "CANDLES::scriptCandles: duplicate name" << set[4];
    return 1;
  }

  Curve *line = candles(data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

int CANDLES::scriptMethod (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,<NAME>,<PENETRATION>
  //    0        1       2       3       4         5

  if (set.count() != 6)
  {
    qDebug() << "CANDLES::scriptMethod: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << "CANDLES::scriptMethod: invalid method" << set[3];
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "CANDLES::scriptMethod: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  double pen = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CANDLES::scriptMethod: invalid penetration" << set[5];
    return 1;
  }

  Curve *line = getMethod(method, pen, data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

Curve * CANDLES::candles (BarData &data)
{
  int size = data.count();

  if (! size)
    return 0;

  Curve *line = new Curve(Curve::Candle);

  int loop;
  for (loop = 0; loop < size; loop++)
  {
    CurveBar *bar = new CurveBar;
    Bar tbar = data.getBar(loop);
    bar->setData(0, tbar.getOpen());
    bar->setData(1, tbar.getHigh());
    bar->setData(2, tbar.getLow());
    bar->setData(3, tbar.getClose());
    line->setBar(loop, bar);
  }

  return line;
}

Curve * CANDLES::getMethod (int method, double pen, BarData &data)
{
  int size = data.count();

  if (size < 1)
    return 0;

  TA_Integer out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _ABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0,
                               size - 1,
                               data.getTAData(BarData::Open),
                               data.getTAData(BarData::High),
                               data.getTAData(BarData::Low),
                               data.getTAData(BarData::Close),
                               pen,
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _DARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0,
                                size - 1,
                                data.getTAData(BarData::Open),
                                data.getTAData(BarData::High),
                                data.getTAData(BarData::Low),
                                data.getTAData(BarData::Close),
                                pen,
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _EVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0,
                                 size - 1,
                                 data.getTAData(BarData::Open),
                                 data.getTAData(BarData::High),
                                 data.getTAData(BarData::Low),
                                 data.getTAData(BarData::Close),
                                 pen,
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _EVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0,
                             size - 1,
                             data.getTAData(BarData::Open),
                             data.getTAData(BarData::High),
                             data.getTAData(BarData::Low),
                             data.getTAData(BarData::Close),
                             pen,
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _MORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0,
                                 size - 1,
                                 data.getTAData(BarData::Open),
                                 data.getTAData(BarData::High),
                                 data.getTAData(BarData::Low),
                                 data.getTAData(BarData::Close),
                                 pen,
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _2CROWS:
      rc = TA_CDL2CROWS(0,
                        size - 1,
                        data.getTAData(BarData::Open),
                        data.getTAData(BarData::High),
                        data.getTAData(BarData::Low),
                        data.getTAData(BarData::Close),
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _3BLACKCROWS:
      rc = TA_CDL3BLACKCROWS(0,
                             size - 1,
                             data.getTAData(BarData::Open),
                             data.getTAData(BarData::High),
                             data.getTAData(BarData::Low),
                             data.getTAData(BarData::Close),
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _3INSIDE:
      rc = TA_CDL3INSIDE(0,
                         size - 1,
                         data.getTAData(BarData::Open),
                         data.getTAData(BarData::High),
                         data.getTAData(BarData::Low),
                         data.getTAData(BarData::Close),
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _3LINESTRIKE:
      rc = TA_CDL3LINESTRIKE(0,
                             size - 1,
                             data.getTAData(BarData::Open),
                             data.getTAData(BarData::High),
                             data.getTAData(BarData::Low),
                             data.getTAData(BarData::Close),
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _3OUTSIDE:
      rc = TA_CDL3OUTSIDE(0,
                          size - 1,
                          data.getTAData(BarData::Open),
                          data.getTAData(BarData::High),
                          data.getTAData(BarData::Low),
                          data.getTAData(BarData::Close),
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _3STARSINSOUTH:
      rc = TA_CDL3STARSINSOUTH(0,
                               size - 1,
                               data.getTAData(BarData::Open),
                               data.getTAData(BarData::High),
                               data.getTAData(BarData::Low),
                               data.getTAData(BarData::Close),
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _3WHITESOLDIERS:
      rc = TA_CDL3WHITESOLDIERS(0,
                                size - 1,
                                data.getTAData(BarData::Open),
                                data.getTAData(BarData::High),
                                data.getTAData(BarData::Low),
                                data.getTAData(BarData::Close),
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _ADVANCEBLOCK:
      rc = TA_CDLADVANCEBLOCK(0,
                              size - 1,
                              data.getTAData(BarData::Open),
                              data.getTAData(BarData::High),
                              data.getTAData(BarData::Low),
                              data.getTAData(BarData::Close),
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _BELTHOLD:
      rc = TA_CDLBELTHOLD(0,
                          size - 1,
                          data.getTAData(BarData::Open),
                          data.getTAData(BarData::High),
                          data.getTAData(BarData::Low),
                          data.getTAData(BarData::Close),
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _BREAKAWAY:
      rc = TA_CDLBREAKAWAY(0,
                           size - 1,
                           data.getTAData(BarData::Open),
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _CLOSINGMARUBOZU:
      rc = TA_CDLCLOSINGMARUBOZU(0,
                                 size - 1,
                                 data.getTAData(BarData::Open),
                                 data.getTAData(BarData::High),
                                 data.getTAData(BarData::Low),
                                 data.getTAData(BarData::Close),
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _CONCEALBABYSWALL:
      rc = TA_CDLCONCEALBABYSWALL(0,
                                  size - 1,
                                  data.getTAData(BarData::Open),
                                  data.getTAData(BarData::High),
                                  data.getTAData(BarData::Low),
                                  data.getTAData(BarData::Close),
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    case _COUNTERATTACK:
      rc = TA_CDLCOUNTERATTACK(0,
                               size - 1,
                               data.getTAData(BarData::Open),
                               data.getTAData(BarData::High),
                               data.getTAData(BarData::Low),
                               data.getTAData(BarData::Close),
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _DOJI:
      rc = TA_CDLDOJI(0,
                      size - 1,
                      data.getTAData(BarData::Open),
                      data.getTAData(BarData::High),
                      data.getTAData(BarData::Low),
                      data.getTAData(BarData::Close),
                      &outBeg,
                      &outNb,
                      &out[0]);
      break;
    case _DOJISTAR:
      rc = TA_CDLDOJISTAR(0,
                          size - 1,
                          data.getTAData(BarData::Open),
                          data.getTAData(BarData::High),
                          data.getTAData(BarData::Low),
                          data.getTAData(BarData::Close),
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _DRAGONFLYDOJI:
      rc = TA_CDLDRAGONFLYDOJI(0,
                               size - 1,
                               data.getTAData(BarData::Open),
                               data.getTAData(BarData::High),
                               data.getTAData(BarData::Low),
                               data.getTAData(BarData::Close),
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _ENGULFING:
      rc = TA_CDLENGULFING(0,
                           size - 1,
                           data.getTAData(BarData::Open),
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _GAPSIDESIDEWHITE:
      rc = TA_CDLGAPSIDESIDEWHITE(0,
                                  size - 1,
                                  data.getTAData(BarData::Open),
                                  data.getTAData(BarData::High),
                                  data.getTAData(BarData::Low),
                                  data.getTAData(BarData::Close),
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    case _GRAVESTONEDOJI:
      rc = TA_CDLGRAVESTONEDOJI(0,
                                size - 1,
                                data.getTAData(BarData::Open),
                                data.getTAData(BarData::High),
                                data.getTAData(BarData::Low),
                                data.getTAData(BarData::Close),
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _HAMMER:
      rc = TA_CDLHAMMER(0,
                        size - 1,
                        data.getTAData(BarData::Open),
                        data.getTAData(BarData::High),
                        data.getTAData(BarData::Low),
                        data.getTAData(BarData::Close),
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _HANGINGMAN:
      rc = TA_CDLHANGINGMAN(0,
                            size - 1,
                            data.getTAData(BarData::Open),
                            data.getTAData(BarData::High),
                            data.getTAData(BarData::Low),
                            data.getTAData(BarData::Close),
                            &outBeg,
                            &outNb,
                            &out[0]);
      break;
    case _HARAMI:
      rc = TA_CDLHARAMI(0,
                        size - 1,
                        data.getTAData(BarData::Open),
                        data.getTAData(BarData::High),
                        data.getTAData(BarData::Low),
                        data.getTAData(BarData::Close),
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _HARAMICROSS:
      rc = TA_CDLHARAMICROSS(0,
                             size - 1,
                             data.getTAData(BarData::Open),
                             data.getTAData(BarData::High),
                             data.getTAData(BarData::Low),
                             data.getTAData(BarData::Close),
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _HIGHWAVE:
      rc = TA_CDLHIGHWAVE(0,
                          size - 1,
                          data.getTAData(BarData::Open),
                          data.getTAData(BarData::High),
                          data.getTAData(BarData::Low),
                          data.getTAData(BarData::Close),
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _HIKKAKE:
      rc = TA_CDLHIKKAKE(0,
                         size - 1,
                         data.getTAData(BarData::Open),
                         data.getTAData(BarData::High),
                         data.getTAData(BarData::Low),
                         data.getTAData(BarData::Close),
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _HIKKAKEMOD:
      rc = TA_CDLHIKKAKEMOD(0,
                            size - 1,
                            data.getTAData(BarData::Open),
                            data.getTAData(BarData::High),
                            data.getTAData(BarData::Low),
                            data.getTAData(BarData::Close),
                            &outBeg,
                            &outNb,
                            &out[0]);
      break;
    case _HOMINGPIGEON:
      rc = TA_CDLHOMINGPIGEON(0,
                              size - 1,
                              data.getTAData(BarData::Open),
                              data.getTAData(BarData::High),
                              data.getTAData(BarData::Low),
                              data.getTAData(BarData::Close),
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _IDENTICAL3CROWS:
      rc = TA_CDLIDENTICAL3CROWS(0,
                                 size - 1,
                                 data.getTAData(BarData::Open),
                                 data.getTAData(BarData::High),
                                 data.getTAData(BarData::Low),
                                 data.getTAData(BarData::Close),
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _INNECK:
      rc = TA_CDLINNECK(0,
                        size - 1,
                        data.getTAData(BarData::Open),
                        data.getTAData(BarData::High),
                        data.getTAData(BarData::Low),
                        data.getTAData(BarData::Close),
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _INVERTEDHAMMER:
      rc = TA_CDLINVERTEDHAMMER(0,
                                size - 1,
                                data.getTAData(BarData::Open),
                                data.getTAData(BarData::High),
                                data.getTAData(BarData::Low),
                                data.getTAData(BarData::Close),
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _KICKING:
      rc = TA_CDLKICKING(0,
                         size - 1,
                         data.getTAData(BarData::Open),
                         data.getTAData(BarData::High),
                         data.getTAData(BarData::Low),
                         data.getTAData(BarData::Close),
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _KICKINGBYLENGTH:
      rc = TA_CDLKICKINGBYLENGTH(0,
                                 size - 1,
                                 data.getTAData(BarData::Open),
                                 data.getTAData(BarData::High),
                                 data.getTAData(BarData::Low),
                                 data.getTAData(BarData::Close),
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _LADDERBOTTOM:
      rc = TA_CDLLADDERBOTTOM(0,
                              size - 1,
                              data.getTAData(BarData::Open),
                              data.getTAData(BarData::High),
                              data.getTAData(BarData::Low),
                              data.getTAData(BarData::Close),
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _LONGLEGGEDDOJI:
      rc = TA_CDLLONGLEGGEDDOJI(0,
                                size - 1,
                                data.getTAData(BarData::Open),
                                data.getTAData(BarData::High),
                                data.getTAData(BarData::Low),
                                data.getTAData(BarData::Close),
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _LONGLINE:
      rc = TA_CDLLONGLINE(0,
                          size - 1,
                          data.getTAData(BarData::Open),
                          data.getTAData(BarData::High),
                          data.getTAData(BarData::Low),
                          data.getTAData(BarData::Close),
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _MARUBOZU:
      rc = TA_CDLMARUBOZU(0,
                          size - 1,
                          data.getTAData(BarData::Open),
                          data.getTAData(BarData::High),
                          data.getTAData(BarData::Low),
                          data.getTAData(BarData::Close),
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _MATCHINGLOW:
      rc = TA_CDLMATCHINGLOW(0,
                             size - 1,
                             data.getTAData(BarData::Open),
                             data.getTAData(BarData::High),
                             data.getTAData(BarData::Low),
                             data.getTAData(BarData::Close),
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _ONNECK:
      rc = TA_CDLONNECK(0,
                        size - 1,
                        data.getTAData(BarData::Open),
                        data.getTAData(BarData::High),
                        data.getTAData(BarData::Low),
                        data.getTAData(BarData::Close),
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _PIERCING:
      rc = TA_CDLPIERCING(0,
                          size - 1,
                          data.getTAData(BarData::Open),
                          data.getTAData(BarData::High),
                          data.getTAData(BarData::Low),
                          data.getTAData(BarData::Close),
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _RICKSHAWMAN:
      rc = TA_CDLRICKSHAWMAN(0,
                             size - 1,
                             data.getTAData(BarData::Open),
                             data.getTAData(BarData::High),
                             data.getTAData(BarData::Low),
                             data.getTAData(BarData::Close),
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _RISEFALL3METHODS:
      rc = TA_CDLRISEFALL3METHODS(0,
                                  size - 1,
                                  data.getTAData(BarData::Open),
                                  data.getTAData(BarData::High),
                                  data.getTAData(BarData::Low),
                                  data.getTAData(BarData::Close),
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    case _SEPARATINGLINES:
      rc = TA_CDLSEPARATINGLINES(0,
                                 size - 1,
                                 data.getTAData(BarData::Open),
                                 data.getTAData(BarData::High),
                                 data.getTAData(BarData::Low),
                                 data.getTAData(BarData::Close),
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _SHOOTINGSTAR:
      rc = TA_CDLSHOOTINGSTAR(0,
                              size - 1,
                              data.getTAData(BarData::Open),
                              data.getTAData(BarData::High),
                              data.getTAData(BarData::Low),
                              data.getTAData(BarData::Close),
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _SHORTLINE:
      rc = TA_CDLSHORTLINE(0,
                           size - 1,
                           data.getTAData(BarData::Open),
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _SPINNINGTOP:
      rc = TA_CDLSPINNINGTOP(0,
                             size - 1,
                             data.getTAData(BarData::Open),
                             data.getTAData(BarData::High),
                             data.getTAData(BarData::Low),
                             data.getTAData(BarData::Close),
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _STALLEDPATTERN:
      rc = TA_CDLSTALLEDPATTERN(0,
                                size - 1,
                                data.getTAData(BarData::Open),
                                data.getTAData(BarData::High),
                                data.getTAData(BarData::Low),
                                data.getTAData(BarData::Close),
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _STICKSANDWICH:
      rc = TA_CDLSTICKSANDWICH(0,
                               size - 1,
                               data.getTAData(BarData::Open),
                               data.getTAData(BarData::High),
                               data.getTAData(BarData::Low),
                               data.getTAData(BarData::Close),
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _TAKURI:
      rc = TA_CDLTAKURI(0,
                        size - 1,
                        data.getTAData(BarData::Open),
                        data.getTAData(BarData::High),
                        data.getTAData(BarData::Low),
                        data.getTAData(BarData::Close),
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _TASUKIGAP:
      rc = TA_CDLTASUKIGAP(0,
                           size - 1,
                           data.getTAData(BarData::Open),
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _THRUSTING:
      rc = TA_CDLTHRUSTING(0,
                           size - 1,
                           data.getTAData(BarData::Open),
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _TRISTAR:
      rc = TA_CDLTRISTAR(0,
                         size - 1,
                         data.getTAData(BarData::Open),
                         data.getTAData(BarData::High),
                         data.getTAData(BarData::Low),
                         data.getTAData(BarData::Close),
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _UNIQUE3RIVER:
      rc = TA_CDLUNIQUE3RIVER(0,
                              size - 1,
                              data.getTAData(BarData::Open),
                              data.getTAData(BarData::High),
                              data.getTAData(BarData::Low),
                              data.getTAData(BarData::Close),
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _UPSIDEGAP2CROWS:
      rc = TA_CDLUPSIDEGAP2CROWS(0,
                                 size - 1,
                                 data.getTAData(BarData::Open),
                                 data.getTAData(BarData::High),
                                 data.getTAData(BarData::Low),
                                 data.getTAData(BarData::Close),
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _XSIDEGAP3METHODS:
      rc = TA_CDLXSIDEGAP3METHODS(0,
                                  size - 1,
                                  data.getTAData(BarData::Open),
                                  data.getTAData(BarData::High),
                                  data.getTAData(BarData::Low),
                                  data.getTAData(BarData::Close),
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "CANDLES::method: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (dataLoop > -1 && outLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

QStringList & CANDLES::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CANDLES *o = new CANDLES;
  return ((IndicatorPlugin *) o);
}

