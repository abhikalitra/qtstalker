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

#include "AD.h"
#include "EMA.h"
#include "PlotFactory.h"

#include <QtDebug>

AD::AD ()
{
  indicator = "AD";

  settings.setData(Method, QString("AD"));
  settings.setData(ADColor, QString("red"));
  settings.setData(ADPlot, QString("Line"));
  settings.setData(ADLabel, QString("AD"));
  settings.setData(FastPeriod, 3);
  settings.setData(SlowPeriod, 10);
  settings.setData(OSCColor, QString("red"));
  settings.setData(OSCPlot, QString("Histogram Bar"));
  settings.setData(OSCLabel, QString("ADOSC"));

  methodList << "AD";
  methodList << "ADOSC";
}

int AD::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  switch (method)
  {
    case 1:
    {
      int fp = settings.getInt(FastPeriod);
      int sp = settings.getInt(SlowPeriod);
      
      settings.getData(OSCColor, s);
      QColor color(s);

      settings.getData(OSCPlot, s);
      PlotFactory fac;
      int lineType = fac.typeFromString(s);
      
      PlotLine *line = getADOSC(data, fp, sp, lineType, color);
      if (! line)
	return 1;
      
      settings.getData(OSCLabel, s);
      line->setLabel(s);
      ind.addLine(line);
      break;
    }
    default:
    {
      settings.getData(ADColor, s);
      QColor color(s);

      settings.getData(ADPlot, s);
      PlotFactory fac;
      int lineType = fac.typeFromString(s);

      PlotLine *line = getAD(data, lineType, color);
      if (! line)
	return 1;
      
      settings.getData(ADLabel, s);
      line->setLabel(s);
      ind.addLine(line);
      break;
    }
  }

  return 0;
}

int AD::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  QString s;
  int method = methodList.indexOf(set[2]);

  int rc = 1;
  switch (method)
  {
    case 0:
      rc = getCUS_AD(set, tlines, data);
      break;
    case 1:
      rc = getCUS_ADOSC(set, tlines, data);
      break;
  }

  return rc;
}

int AD::getCUS_AD (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AD,AD,<NAME>,<LINE TYPE>,<COLOR>
  //      0      1     2  3    4       5          6

  if (set.count() != 7)
  {
    qDebug() << indicator << "::getCUS_AD: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS_AD: duplicate name" << set[4];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[5]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS_AD: invalid plot type" << set[5];
    return 1;
  }

  QColor color(set[6]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS_AD: invalid color" << set[6];
    return 1;
  }

  PlotLine *line = getAD(data, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);
  
  tlines.insert(set[4], line);

  return 0;
}

int AD::getCUS_ADOSC (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,AD,ADOSC,<NAME>,<FAST_PERIOD>,<SLOW_PERIOD>,<PLOT TYPE>,<COLOR>
  //      0       1    2    3    4         5             6             7         8

  if (set.count() != 9)
  {
    qDebug() << indicator << "::getCUS_ADOSC: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS_ADOSC: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int fast = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS_ADOSC: invalid fast period" << set[5];
    return 1;
  }

  int slow = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS_ADOSC: invalid slow period" << set[6];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[7]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS_ADOSC: invalid plot type" << set[7];
    return 1;
  }

  QColor color(set[8]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS_ADOSC: invalid color" << set[8];
    return 1;
  }

  PlotLine *line = getADOSC(data, fast, slow, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * AD::getAD (BarData *data, int lineType, QColor &color)
{
  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;
  
  int size = data->count();
  int loop = 0;
  double ad = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    ad += (((bar->getClose() - bar->getLow()) - (bar->getHigh() - bar->getClose())) / (bar->getHigh() - bar->getLow())) * (bar->getVolume());
    line->setData(loop, new PlotLineBar(color, ad));
  }

  return line;
}

PlotLine * AD::getADOSC (BarData *data, int fast, int slow, int lineType, QColor &color)
{
  PlotLine *ad = getAD(data, lineType, color);
  if (! ad)
    return 0;

  EMA ema;
  PlotLine *fma = ema.ema(ad, fast, lineType, color);
  if (! fma)
  {
    delete ad;
    return 0;
  }

  PlotLine *sma = ema.ema(ad, slow, lineType, color);
  if (! sma)
  {
    delete ad;
    delete fma;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
  {
    delete ad;
    delete fma;
    delete sma;
    return 0;
  }

  QList<int> keys;
  sma->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *fbar = fma->data(keys.at(loop));
    if (! fbar)
      continue;
    
    PlotLineBar *sbar = sma->data(keys.at(loop));

    line->setData(keys.at(loop), new PlotLineBar(color, fbar->data() - sbar->data()));
  }

  delete ad;
  delete fma;
  delete sma;
  
  return line;
}

int AD::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  settings.getData(Method, d);
  dialog->addComboItem((int) Method, page, QObject::tr("Method"), methodList, d);

  page++;
  k = QObject::tr("AD");
  dialog->addPage(page, k);

  settings.getData(ADColor, d);
  dialog->addColorItem((int) ADColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);
  
  settings.getData(ADPlot, d);
  dialog->addComboItem((int) ADPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(ADLabel, d);
  dialog->addTextItem((int) ADLabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("OSC");
  dialog->addPage(page, k);

  settings.getData(OSCColor, d);
  dialog->addColorItem(OSCColor, page, QObject::tr("Color"), d);

  settings.getData(OSCPlot, d);
  dialog->addComboItem(OSCPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(OSCLabel, d);
  dialog->addTextItem(OSCLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), settings.getInt(FastPeriod), 1, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), settings.getInt(SlowPeriod), 1, 100000);

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
  AD *o = new AD;
  return ((IndicatorPlugin *) o);
}


