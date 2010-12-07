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
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  _method << "2CROWS";
  _method << "3BLACKCROWS";
  _method << "3INSIDE";
  _method << "3LINESTRIKE";
  _method << "3OUTSIDE";
  _method << "3STARSINSOUTH";
  _method << "3WHITESOLDIERS";
  _method << "ABANDONEDBABY";
  _method << "ADVANCEBLOCK";
  _method << "BELTHOLD";
  _method << "BREAKAWAY";
  _method << "CLOSINGMARUBOZU";
  _method << "CONCEALBABYSWALL";
  _method << "COUNTERATTACK";
  _method << "DARKCLOUDCOVER";
  _method << "DOJI";
  _method << "DOJISTAR";
  _method << "DRAGONFLYDOJI";
  _method << "ENGULFING";
  _method << "EVENINGDOJISTAR";
  _method << "EVENINGSTAR";
  _method << "GAPSIDESIDEWHITE";
  _method << "GRAVESTONEDOJI";
  _method << "HAMMER";
  _method << "HANGINGMAN";
  _method << "HARAMI";
  _method << "HARAMICROSS";
  _method << "HIGHWAVE";
  _method << "HIKKAKE";
  _method << "HIKKAKEMOD";
  _method << "HOMINGPIGEON";
  _method << "IDENTICAL3CROWS";
  _method << "INNECK";
  _method << "INVERTEDHAMMER";
  _method << "KICKING";
  _method << "KICKINGBYLENGTH";
  _method << "LADDERBOTTOM";
  _method << "LONGLEGGEDDOJI";
  _method << "LONGLINE";
  _method << "MARUBOZU";
  _method << "MATCHINGLOW";
  _method << "MATHOLD";
  _method << "MORNINGDOJISTAR";
  _method << "MORNINGSTAR";
  _method << "ONNECK";
  _method << "PIERCING";
  _method << "RICKSHAWMAN";
  _method << "RISEFALL3METHODS";
  _method << "SEPARATINGLINES";
  _method << "SHOOTINGSTAR";
  _method << "SHORTLINE";
  _method << "SPINNINGTOP";
  _method << "STALLEDPATTERN";
  _method << "STICKSANDWICH";
  _method << "TAKURI";
  _method << "TASUKIGAP";
  _method << "THRUSTING";
  _method << "TRISTAR";
  _method << "UNIQUE3RIVER";
  _method << "UPSIDEGAP2CROWS";
  _method << "XSIDEGAP3METHODS";
}

int CANDLES::command (Command *command)
{
  // CANDLES,<METHOD>,<NAME>
  //  0         1       2

  // CANDLES,<METHOD>,<NAME>,<PENETRATION>
  //  0         1       2         3

  if (command->count() < 3)
  {
    qDebug() << "CANDLES::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _2CROWS:
    case _3BLACKCROWS:
    case _3INSIDE:
    case _3LINESTRIKE:
    case _3OUTSIDE:
    case _3STARSINSOUTH:
    case _3WHITESOLDIERS:
    case _ADVANCEBLOCK:
    case _BELTHOLD:
    case _BREAKAWAY:
    case _CLOSINGMARUBOZU:
    case _CONCEALBABYSWALL:
    case _COUNTERATTACK:
    case _DOJI:
    case _DOJISTAR:
    case _DRAGONFLYDOJI:
    case _ENGULFING:
    case _GAPSIDESIDEWHITE:
    case _GRAVESTONEDOJI:
    case _HAMMER:
    case _HANGINGMAN:
    case _HARAMI:
    case _HARAMICROSS:
    case _HIGHWAVE:
    case _HIKKAKE:
    case _HIKKAKEMOD:
    case _HOMINGPIGEON:
    case _IDENTICAL3CROWS:
    case _INNECK:
    case _INVERTEDHAMMER:
    case _KICKING:
    case _KICKINGBYLENGTH:
    case _LADDERBOTTOM:
    case _LONGLEGGEDDOJI:
    case _LONGLINE:
    case _MARUBOZU:
    case _MATCHINGLOW:
    case _MATHOLD:
    case _MORNINGSTAR:
    case _ONNECK:
    case _PIERCING:
    case _RICKSHAWMAN:
    case _RISEFALL3METHODS:
    case _SEPARATINGLINES:
    case _SHOOTINGSTAR:
    case _SHORTLINE:
    case _SPINNINGTOP:
    case _STALLEDPATTERN:
    case _STICKSANDWICH:
    case _TAKURI:
    case _TASUKIGAP:
    case _THRUSTING:
    case _TRISTAR:
    case _UNIQUE3RIVER:
    case _UPSIDEGAP2CROWS:
    case _XSIDEGAP3METHODS:
      return getCandles(command);
      break;
    case _ABANDONEDBABY:
    case _DARKCLOUDCOVER:
    case _EVENINGDOJISTAR:
    case _EVENINGSTAR:
    case _MORNINGDOJISTAR:
      return getCandlesPen(command);
      break;
    default:
      break;
  }

  return 0;
}

int CANDLES::getCandles (Command *command)
{
  // CANDLES,<METHOD>,<NAME>
  //    0       1       2

  if (command->count() != 3)
  {
    qDebug() << "CANDLES::getCandles: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CANDLES::getCandles: no indicator";
    return 1;
  }

  BarData *data = command->barData();
  if (! data)
  {
    qDebug() << "CANDLES::getCandles: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  QString name = command->parm(2);

  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "CANDLES::getCandles: duplicate name" << name;
    return 1;
  }

  int size = data->count();
  TA_Integer out[size];
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->bar(loop);
    open[loop] = (TA_Real) bar->open();
    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
    close[loop] = (TA_Real) bar->close();
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _2CROWS:
      rc = TA_CDL2CROWS(0,
                        size - 1,
                        &open[0],
                        &high[0],
                        &low[0],
                        &close[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _3BLACKCROWS:
      rc = TA_CDL3BLACKCROWS(0,
                             size - 1,
                             &open[0],
                             &high[0],
                             &low[0],
                             &close[0],
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _3INSIDE:
      rc = TA_CDL3INSIDE(0,
                         size - 1,
                         &open[0],
                         &high[0],
                         &low[0],
                         &close[0],
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _3LINESTRIKE:
      rc = TA_CDL3LINESTRIKE(0,
                             size - 1,
                             &open[0],
                             &high[0],
                             &low[0],
                             &close[0],
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _3OUTSIDE:
      rc = TA_CDL3OUTSIDE(0,
                          size - 1,
                          &open[0],
                          &high[0],
                          &low[0],
                          &close[0],
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _3STARSINSOUTH:
      rc = TA_CDL3STARSINSOUTH(0,
                               size - 1,
                               &open[0],
                               &high[0],
                               &low[0],
                               &close[0],
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _3WHITESOLDIERS:
      rc = TA_CDL3WHITESOLDIERS(0,
                                size - 1,
                                &open[0],
                                &high[0],
                                &low[0],
                                &close[0],
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _ADVANCEBLOCK:
      rc = TA_CDLADVANCEBLOCK(0,
                              size - 1,
                              &open[0],
                              &high[0],
                              &low[0],
                              &close[0],
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _BELTHOLD:
      rc = TA_CDLBELTHOLD(0,
                          size - 1,
                          &open[0],
                          &high[0],
                          &low[0],
                          &close[0],
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _BREAKAWAY:
      rc = TA_CDLBREAKAWAY(0,
                           size - 1,
                           &open[0],
                           &high[0],
                           &low[0],
                           &close[0],
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _CLOSINGMARUBOZU:
      rc = TA_CDLCLOSINGMARUBOZU(0,
                                 size - 1,
                                 &open[0],
                                 &high[0],
                                 &low[0],
                                 &close[0],
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _CONCEALBABYSWALL:
      rc = TA_CDLCONCEALBABYSWALL(0,
                                  size - 1,
                                  &open[0],
                                  &high[0],
                                  &low[0],
                                  &close[0],
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    case _COUNTERATTACK:
      rc = TA_CDLCOUNTERATTACK(0,
                               size - 1,
                               &open[0],
                               &high[0],
                               &low[0],
                               &close[0],
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _DOJI:
      rc = TA_CDLDOJI(0,
                      size - 1,
                      &open[0],
                      &high[0],
                      &low[0],
                      &close[0],
                      &outBeg,
                      &outNb,
                      &out[0]);
      break;
    case _DOJISTAR:
      rc = TA_CDLDOJISTAR(0,
                          size - 1,
                          &open[0],
                          &high[0],
                          &low[0],
                          &close[0],
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _DRAGONFLYDOJI:
      rc = TA_CDLDRAGONFLYDOJI(0,
                               size - 1,
                               &open[0],
                               &high[0],
                               &low[0],
                               &close[0],
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _ENGULFING:
      rc = TA_CDLENGULFING(0,
                           size - 1,
                           &open[0],
                           &high[0],
                           &low[0],
                           &close[0],
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _GAPSIDESIDEWHITE:
      rc = TA_CDLGAPSIDESIDEWHITE(0,
                                  size - 1,
                                  &open[0],
                                  &high[0],
                                  &low[0],
                                  &close[0],
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    case _GRAVESTONEDOJI:
      rc = TA_CDLGRAVESTONEDOJI(0,
                                size - 1,
                                &open[0],
                                &high[0],
                                &low[0],
                                &close[0],
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _HAMMER:
      rc = TA_CDLHAMMER(0,
                        size - 1,
                        &open[0],
                        &high[0],
                        &low[0],
                        &close[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _HANGINGMAN:
      rc = TA_CDLHANGINGMAN(0,
                            size - 1,
                            &open[0],
                            &high[0],
                            &low[0],
                            &close[0],
                            &outBeg,
                            &outNb,
                            &out[0]);
      break;
    case _HARAMI:
      rc = TA_CDLHARAMI(0,
                        size - 1,
                        &open[0],
                        &high[0],
                        &low[0],
                        &close[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _HARAMICROSS:
      rc = TA_CDLHARAMICROSS(0,
                             size - 1,
                             &open[0],
                             &high[0],
                             &low[0],
                             &close[0],
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _HIGHWAVE:
      rc = TA_CDLHIGHWAVE(0,
                          size - 1,
                          &open[0],
                          &high[0],
                          &low[0],
                          &close[0],
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _HIKKAKE:
      rc = TA_CDLHIKKAKE(0,
                         size - 1,
                         &open[0],
                         &high[0],
                         &low[0],
                         &close[0],
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _HIKKAKEMOD:
      rc = TA_CDLHIKKAKEMOD(0,
                            size - 1,
                            &open[0],
                            &high[0],
                            &low[0],
                            &close[0],
                            &outBeg,
                            &outNb,
                            &out[0]);
      break;
    case _HOMINGPIGEON:
      rc = TA_CDLHOMINGPIGEON(0,
                              size - 1,
                              &open[0],
                              &high[0],
                              &low[0],
                              &close[0],
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _IDENTICAL3CROWS:
      rc = TA_CDLIDENTICAL3CROWS(0,
                                 size - 1,
                                 &open[0],
                                 &high[0],
                                 &low[0],
                                 &close[0],
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _INNECK:
      rc = TA_CDLINNECK(0,
                        size - 1,
                        &open[0],
                        &high[0],
                        &low[0],
                        &close[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _INVERTEDHAMMER:
      rc = TA_CDLINVERTEDHAMMER(0,
                                size - 1,
                                &open[0],
                                &high[0],
                                &low[0],
                                &close[0],
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _KICKING:
      rc = TA_CDLKICKING(0,
                         size - 1,
                         &open[0],
                         &high[0],
                         &low[0],
                         &close[0],
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _KICKINGBYLENGTH:
      rc = TA_CDLKICKINGBYLENGTH(0,
                                 size - 1,
                                 &open[0],
                                 &high[0],
                                 &low[0],
                                 &close[0],
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _LADDERBOTTOM:
      rc = TA_CDLLADDERBOTTOM(0,
                              size - 1,
                              &open[0],
                              &high[0],
                              &low[0],
                              &close[0],
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _LONGLEGGEDDOJI:
      rc = TA_CDLLONGLEGGEDDOJI(0,
                                size - 1,
                                &open[0],
                                &high[0],
                                &low[0],
                                &close[0],
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _LONGLINE:
      rc = TA_CDLLONGLINE(0,
                          size - 1,
                          &open[0],
                          &high[0],
                          &low[0],
                          &close[0],
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _MARUBOZU:
      rc = TA_CDLMARUBOZU(0,
                          size - 1,
                          &open[0],
                          &high[0],
                          &low[0],
                          &close[0],
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _MATCHINGLOW:
      rc = TA_CDLMATCHINGLOW(0,
                             size - 1,
                             &open[0],
                             &high[0],
                             &low[0],
                             &close[0],
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _ONNECK:
      rc = TA_CDLONNECK(0,
                        size - 1,
                        &open[0],
                        &high[0],
                        &low[0],
                        &close[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _PIERCING:
      rc = TA_CDLPIERCING(0,
                          size - 1,
                          &open[0],
                          &high[0],
                          &low[0],
                          &close[0],
                          &outBeg,
                          &outNb,
                          &out[0]);
      break;
    case _RICKSHAWMAN:
      rc = TA_CDLRICKSHAWMAN(0,
                             size - 1,
                             &open[0],
                             &high[0],
                             &low[0],
                             &close[0],
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _RISEFALL3METHODS:
      rc = TA_CDLRISEFALL3METHODS(0,
                                  size - 1,
                                  &open[0],
                                  &high[0],
                                  &low[0],
                                  &close[0],
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    case _SEPARATINGLINES:
      rc = TA_CDLSEPARATINGLINES(0,
                                 size - 1,
                                 &open[0],
                                 &high[0],
                                 &low[0],
                                 &close[0],
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _SHOOTINGSTAR:
      rc = TA_CDLSHOOTINGSTAR(0,
                              size - 1,
                              &open[0],
                              &high[0],
                              &low[0],
                              &close[0],
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _SHORTLINE:
      rc = TA_CDLSHORTLINE(0,
                           size - 1,
                           &open[0],
                           &high[0],
                           &low[0],
                           &close[0],
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _SPINNINGTOP:
      rc = TA_CDLSPINNINGTOP(0,
                             size - 1,
                             &open[0],
                             &high[0],
                             &low[0],
                             &close[0],
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _STALLEDPATTERN:
      rc = TA_CDLSTALLEDPATTERN(0,
                                size - 1,
                                &open[0],
                                &high[0],
                                &low[0],
                                &close[0],
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _STICKSANDWICH:
      rc = TA_CDLSTICKSANDWICH(0,
                               size - 1,
                               &open[0],
                               &high[0],
                               &low[0],
                               &close[0],
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _TAKURI:
      rc = TA_CDLTAKURI(0,
                        size - 1,
                        &open[0],
                        &high[0],
                        &low[0],
                        &close[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
      break;
    case _TASUKIGAP:
      rc = TA_CDLTASUKIGAP(0,
                           size - 1,
                           &open[0],
                           &high[0],
                           &low[0],
                           &close[0],
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _THRUSTING:
      rc = TA_CDLTHRUSTING(0,
                           size - 1,
                           &open[0],
                           &high[0],
                           &low[0],
                           &close[0],
                           &outBeg,
                           &outNb,
                           &out[0]);
      break;
    case _TRISTAR:
      rc = TA_CDLTRISTAR(0,
                         size - 1,
                         &open[0],
                         &high[0],
                         &low[0],
                         &close[0],
                         &outBeg,
                         &outNb,
                         &out[0]);
      break;
    case _UNIQUE3RIVER:
      rc = TA_CDLUNIQUE3RIVER(0,
                              size - 1,
                              &open[0],
                              &high[0],
                              &low[0],
                              &close[0],
                              &outBeg,
                              &outNb,
                              &out[0]);
      break;
    case _UPSIDEGAP2CROWS:
      rc = TA_CDLUPSIDEGAP2CROWS(0,
                                 size - 1,
                                 &open[0],
                                 &high[0],
                                 &low[0],
                                 &close[0],
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _XSIDEGAP3METHODS:
      rc = TA_CDLXSIDEGAP3METHODS(0,
                                  size - 1,
                                  &open[0],
                                  &high[0],
                                  &low[0],
                                  &close[0],
                                  &outBeg,
                                  &outNb,
                                  &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "CANDLES::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (dataLoop > -1 && outLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setLabel(name);

  i->setLine(name, line);

  command->setReturnData("0");

  return 0;
}

int CANDLES::getCandlesPen (Command *command)
{
  // CANDLES,<METHOD>,<NAME>,<PENETRATION>
  //    0       1       2          3

  if (command->count() != 4)
  {
    qDebug() << "CANDLES::getCandlesPen: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CANDLES::getCandlesPen: no indicator";
    return 1;
  }

  BarData *data = command->barData();
  if (! data)
  {
    qDebug() << "CANDLES::getCandlesPen: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  int pos = 2;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "CANDLES::getCandlesPen: duplicate name" << name;
    return 1;
  }

  pos++;
  bool ok;
  double pen = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CANDLES::getCandlesPen: invalid penetration" << command->parm(pos);
    return 1;
  }

  int size = data->count();
  TA_Integer out[size];
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->bar(loop);
    open[loop] = (TA_Real) bar->open();
    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
    close[loop] = (TA_Real) bar->close();
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _ABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0,
                               size - 1,
                               &open[0],
                               &high[0],
                               &low[0],
                               &close[0],
                               pen,
                               &outBeg,
                               &outNb,
                               &out[0]);
      break;
    case _DARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0,
                                size - 1,
                                &open[0],
                                &high[0],
                                &low[0],
                                &close[0],
                                pen,
                                &outBeg,
                                &outNb,
                                &out[0]);
      break;
    case _EVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0,
                                 size - 1,
                                 &open[0],
                                 &high[0],
                                 &low[0],
                                 &close[0],
                                 pen,
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    case _EVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0,
                             size - 1,
                             &open[0],
                             &high[0],
                             &low[0],
                             &close[0],
                             pen,
                             &outBeg,
                             &outNb,
                             &out[0]);
      break;
    case _MORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0,
                                 size - 1,
                                 &open[0],
                                 &high[0],
                                 &low[0],
                                 &close[0],
                                 pen,
                                 &outBeg,
                                 &outNb,
                                 &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "CANDLES::getCandlesPen: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (dataLoop > -1 && outLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setLabel(name);

  i->setLine(name, line);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CANDLES *o = new CANDLES;
  return ((ScriptPlugin *) o);
}
