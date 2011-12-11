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

#include "CommandCandlePattern.h"
#include "CandleType.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "TALibInput.h"
#include "TALibOutput.h"
#include "DataString.h"
#include "DataDouble.h"
#include "DataList.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <QList>

CommandCandlePattern::CommandCandlePattern (QObject *p) : Command (p)
{
  _name = "CANDLE_PATTERN";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeOpen, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypePenetration, new DataDouble(50));

  DataList *dl = new DataList;
  CandleType ct;
  dl->set(ct.list());
  _values.insert(_ParmTypeMethod, dl);

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "CommandCandlePattern::CommandCandlePattern: error on TA_Initialize";
}

void CommandCandlePattern::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandCandlePattern::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *iopen = svc.toCurve(script, _values.value(_ParmTypeOpen)->toString(), toffset);
  if (! iopen)
  {
    Command::error("CommandCandlePattern::runScript: invalid Open");
    return;
  }

  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandCandlePattern::runScript: invalid High");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandCandlePattern::runScript: invalid Low");
    return;
  }

  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandCandlePattern::runScript: invalid Close");
    return;
  }

  QList<Data *> list;
  list << iopen << ihigh << ilow << iclose;

  Data *line = getPattern(list,
			  _values.value(_ParmTypeMethod)->toInteger(),
			  _values.value(_ParmTypePenetration)->toDouble());
  if (! line)
  {
    Command::error("CommandCandlePattern::runScript: getPattern error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandCandlePattern::getPattern (QList<Data *> &list, int type, double pen)
{
  switch ((CandleType::Method) type)
  {
    case CandleType::_ABANDONEDBABY:
    case CandleType::_DARKCLOUDCOVER:
    case CandleType::_EVENINGDOJISTAR:
    case CandleType::_EVENINGSTAR:
    case CandleType::_MORNINGDOJISTAR:
      return getCandlesPen(list, type, pen);
      break;
    default:
      return getCandles(list, type);
      break;
  }

  return 0;
}

Data * CommandCandlePattern::getCandles (QList<Data *> &list, int type)
{
  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Integer out[size];
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill4(list, keys, &open[0], &high[0], &low[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;

  switch ((CandleType::Method) type)
  {
    case CandleType::_2CROWS:
      rc = TA_CDL2CROWS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_3BLACKCROWS:
      rc = TA_CDL3BLACKCROWS(0,  size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_3INSIDE:
      rc = TA_CDL3INSIDE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_3LINESTRIKE:
      rc = TA_CDL3LINESTRIKE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_3OUTSIDE:
      rc = TA_CDL3OUTSIDE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_3STARSINSOUTH:
      rc = TA_CDL3STARSINSOUTH(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_3WHITESOLDIERS:
      rc = TA_CDL3WHITESOLDIERS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_ADVANCEBLOCK:
      rc = TA_CDLADVANCEBLOCK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_BELTHOLD:
      rc = TA_CDLBELTHOLD(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_BREAKAWAY:
      rc = TA_CDLBREAKAWAY(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_CLOSINGMARUBOZU:
      rc = TA_CDLCLOSINGMARUBOZU(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_CONCEALBABYSWALL:
      rc = TA_CDLCONCEALBABYSWALL(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_COUNTERATTACK:
      rc = TA_CDLCOUNTERATTACK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_DOJI:
      rc = TA_CDLDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_DOJISTAR:
      rc = TA_CDLDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_DRAGONFLYDOJI:
      rc = TA_CDLDRAGONFLYDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_ENGULFING:
      rc = TA_CDLENGULFING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_GAPSIDESIDEWHITE:
      rc = TA_CDLGAPSIDESIDEWHITE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_GRAVESTONEDOJI:
      rc = TA_CDLGRAVESTONEDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HAMMER:
      rc = TA_CDLHAMMER(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HANGINGMAN:
      rc = TA_CDLHANGINGMAN(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HARAMI:
      rc = TA_CDLHARAMI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HARAMICROSS:
      rc = TA_CDLHARAMICROSS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HIGHWAVE:
      rc = TA_CDLHIGHWAVE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HIKKAKE:
      rc = TA_CDLHIKKAKE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HIKKAKEMOD:
      rc = TA_CDLHIKKAKEMOD(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_HOMINGPIGEON:
      rc = TA_CDLHOMINGPIGEON(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_IDENTICAL3CROWS:
      rc = TA_CDLIDENTICAL3CROWS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_INNECK:
      rc = TA_CDLINNECK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_INVERTEDHAMMER:
      rc = TA_CDLINVERTEDHAMMER(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_KICKING:
      rc = TA_CDLKICKING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_KICKINGBYLENGTH:
      rc = TA_CDLKICKINGBYLENGTH(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_LADDERBOTTOM:
      rc = TA_CDLLADDERBOTTOM(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_LONGLEGGEDDOJI:
      rc = TA_CDLLONGLEGGEDDOJI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_LONGLINE:
      rc = TA_CDLLONGLINE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_MARUBOZU:
      rc = TA_CDLMARUBOZU(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_MATCHINGLOW:
      rc = TA_CDLMATCHINGLOW(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_ONNECK:
      rc = TA_CDLONNECK(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_PIERCING:
      rc = TA_CDLPIERCING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_RICKSHAWMAN:
      rc = TA_CDLRICKSHAWMAN(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_RISEFALL3METHODS:
      rc = TA_CDLRISEFALL3METHODS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_SEPARATINGLINES:
      rc = TA_CDLSEPARATINGLINES(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_SHOOTINGSTAR:
      rc = TA_CDLSHOOTINGSTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_SHORTLINE:
      rc = TA_CDLSHORTLINE(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_SPINNINGTOP:
      rc = TA_CDLSPINNINGTOP(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_STALLEDPATTERN:
      rc = TA_CDLSTALLEDPATTERN(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_STICKSANDWICH:
      rc = TA_CDLSTICKSANDWICH(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_TAKURI:
      rc = TA_CDLTAKURI(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_TASUKIGAP:
      rc = TA_CDLTASUKIGAP(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_THRUSTING:
      rc = TA_CDLTHRUSTING(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_TRISTAR:
      rc = TA_CDLTRISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_UNIQUE3RIVER:
      rc = TA_CDLUNIQUE3RIVER(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_UPSIDEGAP2CROWS:
      rc = TA_CDLUPSIDEGAP2CROWS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_XSIDEGAP3METHODS:
      rc = TA_CDLXSIDEGAP3METHODS(0, size - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getCandles: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new CurveData;
  outs.append(c);

  TALibOutput tao;
  if (tao.fillInteger1(outs, keys, outNb, &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

Data * CommandCandlePattern::getCandlesPen (QList<Data *> &list, int type, double pen)
{
  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Integer out[size];
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TALibInput tai;
  size = tai.fill4(list, keys, &open[0], &high[0], &low[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;

  switch ((CandleType::Method) type)
  {
    case CandleType::_ABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_DARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_EVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_EVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case CandleType::_MORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getCandlesPen: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new CurveData;
  outs.append(c);

  TALibOutput tao;
  if (tao.fillInteger1(outs, keys, outNb, &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}
