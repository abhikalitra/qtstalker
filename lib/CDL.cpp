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

#include "CDL.h"

#include <QtDebug>


CDL::CDL ()
{
  penKey = QObject::tr("Penetration");
  methodKey = QObject::tr("Method");

  methodList << "2CROWS";
  methodList << "3BLACKCROWS";
  methodList << "3INSIDE";
  methodList << "3LINESTRIKE";
  methodList << "3OUTSIDE";
  methodList << "3STARSINSOUTH";
  methodList << "3WHITESOLDIERS";
  methodList << "ABANDONEDBABY";
  methodList << "ADVANCEBLOCK";
  methodList << "BELTHOLD";
  methodList << "BREAKAWAY";
  methodList << "CLOSINGMARUBOZU";
  methodList << "CONCEALBABYSWALL";
  methodList << "COUNTERATTACK";
  methodList << "DARKCLOUDCOVER";
  methodList << "DOJI";
  methodList << "DOJISTAR";
  methodList << "DRAGONFLYDOJI";
  methodList << "ENGULFING";
  methodList << "EVENINGDOJISTAR";
  methodList << "EVENINGSTAR";
  methodList << "GAPSIDESIDEWHITE";
  methodList << "GRAVESTONEDOJI";
  methodList << "HAMMER";
  methodList << "HANGINGMAN";
  methodList << "HARAMI";
  methodList << "HARAMICROSS";
  methodList << "HIGHWAVE";
  methodList << "HIKKAKE";
  methodList << "HIKKAKEMOD";
  methodList << "HOMINGPIGEON";
  methodList << "IDENTICAL3CROWS";
  methodList << "INNECK";
  methodList << "INVERTEDHAMMER";
  methodList << "KICKING";
  methodList << "KICKINGBYLENGTH";
  methodList << "LADDERBOTTOM";
  methodList << "LONGLEGGEDDOJI";
  methodList << "LONGLINE";
  methodList << "MARUBOZU";
  methodList << "MATCHINGLOW";
  methodList << "MATHOLD";
  methodList << "MORNINGDOJISTAR";
  methodList << "MORNINGSTAR";
  methodList << "ONNECK";
  methodList << "PIERCING";
  methodList << "RICKSHAWMAN";
  methodList << "RISEFALL3METHODS";
  methodList << "SEPARATINGLINES";
  methodList << "SHOOTINGSTAR";
  methodList << "SHORTLINE";
  methodList << "SPINNINGTOP";
  methodList << "STALLEDPATTERN";
  methodList << "STICKSANDWICH";
  methodList << "TAKURI";
  methodList << "TASUKIGAP";
  methodList << "THRUSTING";
  methodList << "TRISTAR";
  methodList << "UNIQUE3RIVER";
  methodList << "UPSIDEGAP2CROWS";
  methodList << "XSIDEGAP3METHODS";

  QString d;
  d = "red";
  settings.setData(colorKey, d);

  d = "Line";
  settings.setData(plotKey, d);

  settings.setData(labelKey, indicator);

  settings.setData(penKey, 0.3);

  d = "2CROWS";
  settings.setData(methodKey, d);
}

int CDL::getIndicator (Indicator &ind, BarData *data)
{
  double pen = settings.getDouble(penKey);

  QString s;
  settings.getData(methodKey, s);
  int method = methodList.indexOf(s);

  PlotLine *line = getCDL(data, method, pen);
  if (! line)
    return 1;

  settings.getData(colorKey, s);
  line->setColor(s);

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  return 0;
}

int CDL::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // format INDICATOR,CDL,NAME,METHOD,PENETRATION

  if (set.count() != 5)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[2];
    return 1;
  }

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[3];
    return 1;
  }

  double pen = 0;
  bool ok;
  pen = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid penetration" << set[4];
    return 1;
  }

  PlotLine *line = getCDL(data, method, pen);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * CDL::getCDL (BarData *data, int method, double pen)
{
  int size = data->count();
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
    open[loop] = (TA_Real) data->getOpen(loop);
    high[loop] = (TA_Real) data->getHigh(loop);
    low[loop] = (TA_Real) data->getLow(loop);
    close[loop] = (TA_Real) data->getClose(loop);
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((Function) method)
  {
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
    case _ABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
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
    case _DARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
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
    case _EVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _EVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
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
    case _MATHOLD:
      rc = TA_CDLMATHOLD(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _MORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
      break;
    case _MORNINGSTAR:
      rc = TA_CDLMORNINGSTAR(0, size - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &out[0]);
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
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

int CDL::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(colorKey, d);
  dialog->addColorItem(page, colorKey, d);

  settings.getData(plotKey, d);
  dialog->addComboItem(page, plotKey, plotList, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  dialog->addDoubleItem(page, penKey, settings.getDouble(penKey), 0, 100);

  settings.getData(methodKey, d);
  dialog->addComboItem(page, methodKey, methodList, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  dialog->getItem(colorKey, d);
  settings.setData(colorKey, d);

  dialog->getItem(plotKey, d);
  settings.setData(plotKey, d);

  dialog->getItem(labelKey, d);
  settings.setData(labelKey, d);

  dialog->getItem(penKey, d);
  settings.setData(penKey, d);

  dialog->getItem(methodKey, d);
  settings.setData(methodKey, d);

  delete dialog;
  return rc;
}

