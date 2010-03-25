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
#include "ta_libc.h"
#include "MATH1.h"

#include <QtDebug>


CANDLES::CANDLES ()
{
  indicator = "CANDLES";

  settings.setData(Penetration, 0.3);
  settings.setData(Method, "NONE");
  settings.setData(MethodColor, "cyan");
  settings.setData(Color, "green");
  settings.setData(Label, indicator);
  settings.setData(MAColor, "red");
  settings.setData(MA2Color, "yellow");
  settings.setData(MA3Color, "blue");
  settings.setData(MAPlot, "Line");
  settings.setData(MA2Plot, "Line");
  settings.setData(MA3Plot, "Line");
  settings.setData(MALabel, "MA1");
  settings.setData(MA2Label, "MA2");
  settings.setData(MA3Label, "MA3");
  settings.setData(MAPeriod, 20);
  settings.setData(MA2Period, 50);
  settings.setData(MA3Period, 200);
  settings.setData(MAType, "SMA");
  settings.setData(MA2Type, "SMA");
  settings.setData(MA3Type, "SMA");

  methodList << "NONE";
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
}

int CANDLES::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Color, s);
  QColor color(s);

  PlotLine *line = getCANDLES(data, color);
  if (! line)
    return 1;

  line->setColor(color);
  settings.getData(Label, s);
  line->setLabel(s);

  settings.getData(Method, s);
  int method = methodList.indexOf(s);
  if (method != _NONE)
  {
    double pen = settings.getDouble(Penetration);

    PlotLine *line2 = getMethod(data, method, pen);
    if (line2)
    {
      settings.getData(MethodColor, s);
      QColor c(s);

      int loop;
      for (loop = 0; loop < line2->count(); loop++)
      {
        if (line2->getData(loop) > 0)
          line->setColorBar(loop, c);
      }
      
      delete line2;
    }
  }

  ind.addLine(line);
  
  MATH1 m;
  QStringList maList;
  m.getMAList(maList);

  int period = settings.getInt(MAPeriod);
  if (period > 1)
  {
    settings.getData(MAType, s);
    int type = maList.indexOf(s);
    PlotLine *ma = m.getMA(line, period, type);
    if (ma)
    {
      settings.getData(MAColor, s);
      ma->setColor(s);
      settings.getData(MAPlot, s);
      ma->setPlugin(s);
      settings.getData(MALabel, s);
      ma->setLabel(s);
      ind.addLine(ma);
    }
  }

  period = settings.getInt(MA2Period);
  if (period > 1)
  {
    settings.getData(MA2Type, s);
    int type = maList.indexOf(s);
    PlotLine *ma = m.getMA(line, period, type);
    if (ma)
    {
      settings.getData(MA2Color, s);
      ma->setColor(s);
      settings.getData(MA2Plot, s);
      ma->setPlugin(s);
      settings.getData(MA2Label, s);
      ma->setLabel(s);
      ind.addLine(ma);
    }
  }

  period = settings.getInt(MA3Period);
  if (period > 1)
  {
    settings.getData(MA3Type, s);
    int type = maList.indexOf(s);
    PlotLine *ma = m.getMA(line, period, type);
    if (ma)
    {
      settings.getData(MA3Color, s);
      ma->setColor(s);
      settings.getData(MA3Plot, s);
      ma->setPlugin(s);
      settings.getData(MA3Label, s);
      ma->setLabel(s);
      ind.addLine(ma);
    }
  }

  return 0;
}

int CANDLES::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,CANDLES,METHOD,<NAME>,<COLOR>
  // INDICATOR,PLUGIN,CANDLES,METHOD,<NAME>,<PENETRATION>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid parm count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  PlotLine *line = 0;

  switch (method)
  {
    case 0:
    {
      QColor color(set[5]);
      if (! color.isValid())
      {
        qDebug() << indicator << "::calculate: invalid color" << set[5];
        return 1;
      }

      line = getCANDLES(data, color);
      break;
    }
    default:
    {
      bool ok;
      double pen = set[5].toDouble(&ok);
      if (! ok)
      {
        qDebug() << indicator << "::calculate: invalid penetration" << set[5];
        return 1;
      }

      line = getMethod(data, method, pen);
      break;
    }
  }

  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * CANDLES::getCANDLES (BarData *data, QColor &color)
{
  PlotLine *line = new PlotLine;
  QString s = "Candle";
  line->setPlugin(s);

  int loop;
  int size = data->count();
  for (loop = 0; loop < size; loop++)
  {
    PlotLineBar bar;
    Bar *tbar = data->getBar(loop);
    bar.append(tbar->getOpen());
    bar.append(tbar->getHigh());
    bar.append(tbar->getLow());
    bar.append(tbar->getClose());
    bar.setColor(color);
    line->append(bar);
  }

  return line;
}

int CANDLES::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Candle Color"), d);

  settings.getData(MethodColor, d);
  dialog->addColorItem(MethodColor, page, QObject::tr("Method Color"), d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), methodList, d);

  dialog->addDoubleItem(Penetration, page, QObject::tr("Penetration"), settings.getDouble(Penetration), 0, 100);

  page++;
  k = QObject::tr("MA 1");
  dialog->addPage(page, k);

  settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), settings.getInt(MAPeriod), 1, 100000);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);

  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 2");
  dialog->addPage(page, k);

  settings.getData(MA2Color, d);
  dialog->addColorItem(MA2Color, page, QObject::tr("Color"), d);

  settings.getData(MA2Plot, d);
  dialog->addComboItem(MA2Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MA2Label, d);
  dialog->addTextItem(MA2Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(MA2Period, page, QObject::tr("Period"), settings.getInt(MA2Period), 1, 100000);

  settings.getData(MA2Type, d);
  dialog->addComboItem(MA2Type, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 3");
  dialog->addPage(page, k);

  settings.getData(MA3Color, d);
  dialog->addColorItem(MA3Color, page, QObject::tr("Color"), d);

  settings.getData(MA3Plot, d);
  dialog->addComboItem(MA3Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MA3Label, d);
  dialog->addTextItem(MA3Label, page, QObject::tr("Label"), d);

  dialog->addIntItem(MA3Period, page, QObject::tr("Period"), settings.getInt(MA3Period), 1, 100000);

  settings.getData(MA3Type, d);
  dialog->addComboItem(MA3Type, page, QObject::tr("Type"), maList, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}

PlotLine * CANDLES::getMethod (BarData *data, int method, double pen)
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
    Bar *bar = data->getBar(loop);
    open[loop] = (TA_Real) bar->getOpen();
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
    close[loop] = (TA_Real) bar->getClose();
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((Function) method)
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
    qDebug() << indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CANDLES *o = new CANDLES;
  return ((IndicatorPlugin *) o);
}


