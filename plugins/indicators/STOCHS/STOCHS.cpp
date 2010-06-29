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

#include "STOCHS.h"
#include "MAFactory.h"
#include "ta_libc.h"
#include "PlotFactory.h"

#include <QtDebug>

STOCHS::STOCHS ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCHS::error on TA_Initialize");

  _indicator = "STOCHS";

  _settings.setData(SlowKColor, "red");
  _settings.setData(SlowDColor, "yellow");
  _settings.setData(Ref1Color, "white");
  _settings.setData(Ref2Color, "white");
  _settings.setData(SlowKPlot, "Line");
  _settings.setData(SlowDPlot, "Dash");
  _settings.setData(SlowKLabel, "SLOWK");
  _settings.setData(SlowDLabel, "SLOWD");
  _settings.setData(FastKPeriod, 5);
  _settings.setData(SlowKPeriod, 3);
  _settings.setData(SlowDPeriod, 3);
  _settings.setData(SlowKMA, "SMA");
  _settings.setData(SlowDMA, "SMA");
  _settings.setData(Ref1, 20);
  _settings.setData(Ref2, 80);
}

int STOCHS::getIndicator (Indicator &ind, BarData &data)
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

  int fkperiod = _settings.getInt(FastKPeriod);

  _settings.getData(SlowKColor, s);
  QColor kcolor(s);

  _settings.getData(SlowKPlot, s);
  int klineType = fac.typeFromString(s);

  int skperiod = _settings.getInt(SlowKPeriod);

  MAFactory mau;
  _settings.getData(SlowKMA, s);
  int kmaType = mau.typeFromString(s);

  int dperiod = _settings.getInt(SlowDPeriod);

  _settings.getData(SlowDMA, s);
  int dmaType = mau.typeFromString(s);

  _settings.getData(SlowDColor, s);
  QColor dcolor(s);

  _settings.getData(SlowDPlot, s);
  int dlineType = fac.typeFromString(s);

  QList<PlotLine *> pl;
  if (getSTOCHS(data,
                fkperiod,
                skperiod,
                dperiod,
                kmaType,
                dmaType,
                klineType,
                kcolor,
                dlineType,
                dcolor,
                pl))
    return 1;

  line = pl.at(0);
  _settings.getData(SlowKLabel, s);
  line->setLabel(s);
  
  s = "2";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  line = pl.at(1);
  _settings.getData(SlowDLabel, s);
  line->setLabel(s);
  
  s = "3";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  return 0;
}

int STOCHS::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,STOCHS,<NAME SLOWK>,<NAME SLOWD>,<FASTK PERIOD>,<SLOWK PERIOD>,<SLOWK MA TYPE>,<SLOWD PERIOD>,<SLOWD MA TYPE>,<SLOWK PLOT TYPE>,<SLOWD PLOT TYPE>,<SLOWK COLOR>,<SLOWD COLOR>
  //     0       1      2         3            4             5              6               7              8              9                10                11               12            13

  if (set.count() != 14)
  {
    qDebug() << _indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid fastk period" << set[5];
    return 1;
  }

  int skp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid slowk period" << set[6];
    return 1;
  }

  MAFactory mau;
  int kma = mau.typeFromString(set[7]);
  if (kma == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid slowk ma" << set[7];
    return 1;
  }

  int sdp = set[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS: invalid slowd period" << set[8];
    return 1;
  }

  int dma = mau.typeFromString(set[9]);
  if (dma == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid slowd ma" << set[9];
    return 1;
  }

  PlotFactory fac;
  int klineType = fac.typeFromString(set[10]);
  if (klineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid slowk plot type" << set[10];
    return 1;
  }

  int dlineType = fac.typeFromString(set[11]);
  if (dlineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid slowd plot type" << set[11];
    return 1;
  }

  QColor kcolor(set[12]);
  if (! kcolor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid slowk color" << set[12];
    return 1;
  }

  QColor dcolor(set[13]);
  if (! dcolor.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid slowd color" << set[13];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getSTOCHS(data,
                fkp,
                skp,
                sdp,
                kma,
                dma,
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

int STOCHS::getSTOCHS (BarData &data, int fkperiod, int skperiod, int sdperiod, int kma, int dma,
                       int klineType, QColor &kcolor, int dlineType, QColor &dcolor, QList<PlotLine *> &pl)
{
  if (data.count() < fkperiod || data.count() < skperiod)
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

  TA_RetCode rc = TA_STOCH(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           fkperiod,
                           skperiod,
                           (TA_MAType) kma,
                           sdperiod,
                           (TA_MAType) dma,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getSTOCHS: TA-Lib error" << rc;
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

int STOCHS::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("SlowK");
  dialog->addPage(page, k);

  _settings.getData(SlowKColor, d);
  dialog->addColorItem(SlowKColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(SlowKPlot, d);
  dialog->addComboItem(SlowKPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(SlowKLabel, d);
  dialog->addTextItem(SlowKLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(FastKPeriod, page, QObject::tr("FastK Period"), _settings.getInt(FastKPeriod), 1, 100000);

  dialog->addIntItem(SlowKPeriod, page, QObject::tr("Period"), _settings.getInt(SlowKPeriod), 1, 100000);

  MAFactory mau;
  QStringList maList = mau.list();

  _settings.getData(SlowKMA, d);
  dialog->addComboItem(SlowKMA, page, QObject::tr("MA Type"), maList, d);

  page++;
  k = QObject::tr("SlowD");
  dialog->addPage(page, k);

  _settings.getData(SlowDColor, d);
  dialog->addColorItem(SlowDColor, page, QObject::tr("Color"), d);

  _settings.getData(SlowDPlot, d);
  dialog->addComboItem(SlowDPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(SlowDLabel, d);
  dialog->addTextItem(SlowDLabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(SlowDPeriod, page, QObject::tr("Period"), _settings.getInt(SlowDPeriod), 1, 100000);

  _settings.getData(SlowDMA, d);
  dialog->addComboItem(SlowDMA, page, QObject::tr("MA Type"), maList, d);

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
  STOCHS *o = new STOCHS;
  return ((IndicatorPlugin *) o);
}


