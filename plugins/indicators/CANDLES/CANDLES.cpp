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
#include "MAFactory.h"
#include "PlotFactory.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  _indicator = "CANDLES";

  _settings.setData(Penetration, 0.3);
  _settings.setData(Method, "NONE");
  _settings.setData(MethodColor, "cyan");
  _settings.setData(Color, "green");
  _settings.setData(Label, _indicator);
  _settings.setData(MAColor, "red");
  _settings.setData(MA2Color, "yellow");
  _settings.setData(MA3Color, "blue");
  _settings.setData(MAPlot, "Line");
  _settings.setData(MA2Plot, "Line");
  _settings.setData(MA3Plot, "Line");
  _settings.setData(MALabel, "MA1");
  _settings.setData(MA2Label, "MA2");
  _settings.setData(MA3Label, "MA3");
  _settings.setData(MAPeriod, 20);
  _settings.setData(MA2Period, 50);
  _settings.setData(MA3Period, 200);
  _settings.setData(MAType, "SMA");
  _settings.setData(MA2Type, "SMA");
  _settings.setData(MA3Type, "SMA");

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

int CANDLES::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  _settings.getData(Color, s);
  QColor color(s);

  PlotLine *line = getCANDLES(data, color);
  if (! line)
    return 1;

  _settings.getData(Label, s);
  line->setLabel(s);

  _settings.getData(Method, s);
  int method = _methodList.indexOf(s);
  if (method != _NONE)
  {
    double pen = _settings.getDouble(Penetration);

    PlotLine *line2 = getMethod(data, method, pen, (int) PlotFactory::PlotTypeLine, color);
    if (line2)
    {
      _settings.getData(MethodColor, s);
      QColor c(s);

      int loop = 0;
      for (; loop < line2->count(); loop++)
      {
        PlotLineBar *bar2 = line2->data(loop);
        if (bar2->data() > 0)
        {
          PlotLineBar *bar = line->data(loop);
          bar->setColor(c);
        }
      }
      
      delete line2;
    }
  }

  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);
  
  int period = _settings.getInt(MAPeriod);
  if (period > 1)
  {
    MAFactory mau;
    _settings.getData(MAType, s);
    int type = mau.typeFromString(s);

    _settings.getData(MAColor, s);
    QColor color(s);

    _settings.getData(MAPlot, s);
    PlotFactory fac;
    int lineType = fac.typeFromString(s);

    PlotLine *ma = mau.ma(line, period, type, lineType, color);
    if (ma)
    {
      _settings.getData(MALabel, s);
      ma->setLabel(s);
      
      s = "1";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  period = _settings.getInt(MA2Period);
  if (period > 1)
  {
    MAFactory mau;
    _settings.getData(MA2Type, s);
    int type = mau.typeFromString(s);

    _settings.getData(MA2Color, s);
    QColor color(s);

    _settings.getData(MA2Plot, s);
    PlotFactory fac;
    int lineType = fac.typeFromString(s);

    PlotLine *ma = mau.ma(line, period, type, lineType, color);
    if (ma)
    {
      _settings.getData(MA2Label, s);
      ma->setLabel(s);
      
      s = "2";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  period = _settings.getInt(MA3Period);
  if (period > 1)
  {
    MAFactory mau;
    _settings.getData(MA3Type, s);
    int type = mau.typeFromString(s);

    _settings.getData(MA3Color, s);
    QColor color(s);

    _settings.getData(MA3Plot, s);
    PlotFactory fac;
    int lineType = fac.typeFromString(s);

    PlotLine *ma = mau.ma(line, period, type, lineType, color);
    if (ma)
    {
      _settings.getData(MA3Label, s);
      ma->setLabel(s);
      
      s = "3";
      ind.setLine(s, ma);
      ind.addPlotOrder(s);
    }
  }

  return 0;
}

int CANDLES::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,*
  //     0       1      2        3

  if (set.count() < 4)
  {
    qDebug() << _indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  int rc = 1;
  int method = _methodList.indexOf(set[3]);
  switch (method)
  {
    case _NONE:
      rc = getCUSNone(set, ind, data);
      break;
    default:
      rc = getCUSMethod(set, ind, data);
      break;
  }

  return rc;
}

int CANDLES::getCUSNone (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,<NAME>,<COLOR>
  //    0        1       2       3       4       5
  
  if (set.count() != 6)
  {
    qDebug() << _indicator << "::getCUSNone: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUSNone: duplicate name" << set[4];
    return 1;
  }

  QColor color(set[5]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUSNone: invalid color" << set[5];
    return 1;
  }
  
  PlotLine *line = getCANDLES(data, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

int CANDLES::getCUSMethod (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,CANDLES,<METHOD>,<NAME>,<PENETRATION>,<PLOT TYPE>,<COLOR>
  //    0        1       2       3       4         5             6         7

  if (set.count() != 8)
  {
    qDebug() << _indicator << "::getCUSMethod: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << _indicator << "::getCUSMethod: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUSMethod: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  double pen = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUSMethod: invalid penetration" << set[5];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUSMethod: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUSMethod: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getMethod(data, method, pen, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

PlotLine * CANDLES::getCANDLES (BarData *data, QColor &color)
{
  PlotFactory fac;
  QString s = "Candle";
  PlotLine *line = fac.plot(s);
  if (! line)
    return 0;

  int loop;
  int size = data->count();
  for (loop = 0; loop < size; loop++)
  {
    PlotLineBar *bar = new PlotLineBar;
    Bar *tbar = data->getBar(loop);
    bar->setData(0, tbar->getOpen());
    bar->setData(1, tbar->getHigh());
    bar->setData(2, tbar->getLow());
    bar->setData(3, tbar->getClose());
    bar->setColor(color);
    line->setData(loop, bar);
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

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Candle Color"), d);

  _settings.getData(MethodColor, d);
  dialog->addColorItem(MethodColor, page, QObject::tr("Method Color"), d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  _settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), _methodList, d);

  dialog->addDoubleItem(Penetration, page, QObject::tr("Penetration"), _settings.getDouble(Penetration), 0, 100);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  page++;
  k = QObject::tr("MA 1");
  dialog->addPage(page, k);

  _settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  _settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), _settings.getInt(MAPeriod), 1, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);

  _settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 2");
  dialog->addPage(page, k);

  _settings.getData(MA2Color, d);
  dialog->addColorItem(MA2Color, page, QObject::tr("Color"), d);

  _settings.getData(MA2Plot, d);
  dialog->addComboItem(MA2Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MA2Label, d);
  dialog->addTextItem(MA2Label, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MA2Period, page, QObject::tr("Period"), _settings.getInt(MA2Period), 1, 100000);

  _settings.getData(MA2Type, d);
  dialog->addComboItem(MA2Type, page, QObject::tr("Type"), maList, d);

  page++;
  k = QObject::tr("MA 3");
  dialog->addPage(page, k);

  _settings.getData(MA3Color, d);
  dialog->addColorItem(MA3Color, page, QObject::tr("Color"), d);

  _settings.getData(MA3Plot, d);
  dialog->addComboItem(MA3Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MA3Label, d);
  dialog->addTextItem(MA3Label, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MA3Period, page, QObject::tr("Period"), _settings.getInt(MA3Period), 1, 100000);

  _settings.getData(MA3Type, d);
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

PlotLine * CANDLES::getMethod (BarData *data, int method, double pen, int lineType, QColor &color)
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
    qDebug() << _indicator << "::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int dataLoop = data->count() - 1;
  int outLoop = outNb - 1;
  while (dataLoop > -1 && outLoop > -1)
  {
    line->setData(dataLoop, new PlotLineBar(color, out[outLoop]));
    dataLoop--;
    outLoop--;
  }

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


