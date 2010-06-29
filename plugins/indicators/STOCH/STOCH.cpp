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

#include "STOCH.h"
#include "MAFactory.h"
#include "ta_libc.h"
#include "PlotFactory.h"

#include <QtDebug>

STOCH::STOCH ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCH::error on TA_Initialize");

  _indicator = "STOCH";

  _settings.setData(FastKColor, "red");
  _settings.setData(FastDColor, "yellow");
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
  _settings.setData(FastKPlot, "Line");
  _settings.setData(FastDPlot, "Dash");
  _settings.setData(FastKLabel, "FASTK");
  _settings.setData(FastDLabel, "FASTD");
  _settings.setData(FastKPeriod, 5);
  _settings.setData(FastDPeriod, 3);
  _settings.setData(FastDMA, "SMA");
  _settings.setData(Ref1, 20);
  _settings.setData(Ref2, 80);
}

int STOCH::getIndicator (Indicator &ind, BarData &data)
{
  // create first ref line
  QString s = "Horizontal";
  PlotFactory fac;
  PlotLine *line = fac.plot(s);
  if (! line)
    return 1;

  _settings.getData(Ref1Color, s);
  QColor color(s);

  line->setData(0, new PlotLineBar(color, (double) _settings.getInt(Ref1)));
  
  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // create second ref line
  s = "Horizontal";
  line = fac.plot(s);
  if (! line)
    return 1;

  _settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setData(0, new PlotLineBar(color, (double) _settings.getInt(Ref2)));
  
  s = "1";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // create the fastk line
  int kperiod = _settings.getInt(FastKPeriod);

  _settings.getData(FastKColor, s);
  QColor kcolor(s);

  _settings.getData(FastKPlot, s);
  int klineType = fac.typeFromString(s);

  int dperiod = _settings.getInt(FastDPeriod);

  MAFactory mau;
  _settings.getData(FastDMA, s);
  int maType = mau.typeFromString(s);

  _settings.getData(FastDColor, s);
  QColor dcolor(s);

  _settings.getData(FastDPlot, s);
  int dlineType = fac.typeFromString(s);

  QList<PlotLine *> pl;
  if (getSTOCH(data,
               kperiod,
               dperiod,
               maType,
               klineType,
               kcolor,
               dlineType,
               dcolor,
               pl))
    return 1;

  line = pl.at(0);
  _settings.getData(FastKLabel, s);
  line->setLabel(s);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);
  
  line = pl.at(1);
  _settings.getData(FastDLabel, s);
  line->setLabel(s);
  
  s = "3";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int STOCH::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,STOCH,<NAME FASTK>,<NAME FASTD>,<FASTK PERIOD>,<FASTD PERIOD>,<FASTD MA TYPE>,<FASTK PLOT TYPE>,<FASTD PLOT TYPE>,<FASTK COLOR>,<FASTD COLOR>
  //     0        1    2         3            4              5             6              7                 8                9                10            11

  if (set.count() != 12)
  {
    qDebug() << _indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate fastk name" << set[3];
    return 1;
  }

  tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate fastd name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid fastk period" << set[5];
    return 1;
  }

  int fdp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid fastd period" << set[6];
    return 1;
  }

  MAFactory mau;
  int ma = mau.typeFromString(set[7]);
  if (ma == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid fastd ma" << set[7];
    return 1;
  }

  PlotFactory fac;
  int klineType = fac.typeFromString(set[8]);
  if (klineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid fastk plot type" << set[8];
    return 1;
  }

  int dlineType = fac.typeFromString(set[9]);
  if (dlineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid fastd plot type" << set[9];
    return 1;
  }

  QColor kcolor(set[10]);
  if (! kcolor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid fastk color" << set[10];
    return 1;
  }

  QColor dcolor(set[11]);
  if (! dcolor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid fastd color" << set[11];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getSTOCH(data,
               fkp,
               fdp,
               ma,
               klineType,
               kcolor,
               dlineType,
               dcolor,
               pl))
    return 1;

  PlotLine *line = pl.at(0);
  line->setLabel(set[3]);
  ind.setLine(set[3], line);

  line = pl.at(1);
  line->setLabel(set[4]);
  ind.setLine(set[4], line);

  return 0;
}

int STOCH::getSTOCH (BarData &data, int kperiod, int dperiod, int ma, int klineType, QColor &kcolor,
                     int dlineType, QColor &dcolor, QList<PlotLine *> &pl)
{
  if (data.count() < kperiod || data.count() < dperiod)
    return 1;

  int size = data.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar bar = data.getBar(loop);
    high[loop] = (TA_Real) bar.getHigh();
    low[loop] = (TA_Real) bar.getLow();
    close[loop] = (TA_Real) bar.getClose();
  }

  TA_RetCode rc = TA_STOCHF(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            kperiod,
                            dperiod,
                            (TA_MAType) ma,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getSTOCH: TA-Lib error" << rc;
    return 1;
  }

  PlotFactory fac;
  PlotLine *kline = fac.plot(klineType);
  if (! kline)
    return 1;

  PlotLine *dline = fac.plot(dlineType);
  if (! dline)
  {
    delete kline;
    return 1;
  }

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    kline->setData(dataLoop, new PlotLineBar(kcolor, out[outLoop]));
    dline->setData(dataLoop, new PlotLineBar(dcolor, out2[outLoop]));
    dataLoop--;
    outLoop--;
  }

  pl.append(kline);
  pl.append(dline);

  return 0;
}

int STOCH::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("FastK");
  dialog->addPage(page, k);

  _settings.getData(FastKColor, d);
  dialog->addColorItem(FastKColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(FastKPlot, d);
  dialog->addComboItem(FastKPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(FastKLabel, d);
  dialog->addTextItem(FastKLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(FastKPeriod, page, QObject::tr("Period"), _settings.getInt(FastKPeriod), 1, 100000);

  page++;
  k = QObject::tr("FastD");
  dialog->addPage(page, k);

  _settings.getData(FastDColor, d);
  dialog->addColorItem(FastDColor, page, QObject::tr("Color"), d);

  _settings.getData(FastDPlot, d);
  dialog->addComboItem(FastDPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(FastDLabel, d);
  dialog->addTextItem(FastDLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(FastDPeriod, page, QObject::tr("Period"), _settings.getInt(FastDPeriod), 1, 100000);

  MAFactory mau;
  QStringList maList = mau.list();

  _settings.getData(FastDMA, d);
  dialog->addComboItem(FastDMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("Ref");
  dialog->addPage(page, k);

  _settings.getData(Ref1Color, d);
  dialog->addColorItem(Ref1Color, page, QObject::tr("Ref. 1 Color"), d);

  _settings.getData(Ref2Color, d);
  dialog->addColorItem(Ref2Color, page, QObject::tr("Ref. 2 Color"), d);

  dialog->addIntItem(Ref1, page, QObject::tr("Ref. 1"), _settings.getInt(Ref1), 0, 100);

  dialog->addIntItem(Ref2, page, QObject::tr("Ref. 2"), _settings.getInt(Ref2), 0, 100);

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

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}


