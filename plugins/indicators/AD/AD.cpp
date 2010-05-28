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
#include "PlotFactory.h"
#include "ta_libc.h"

#include <QtDebug>

AD::AD ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("AD::error on TA_Initialize");

  _indicator = "AD";

  _settings.setData(Method, QString("AD"));
  _settings.setData(ADColor, QString("red"));
  _settings.setData(ADPlot, QString("Line"));
  _settings.setData(ADLabel, QString("AD"));
  _settings.setData(FastPeriod, 3);
  _settings.setData(SlowPeriod, 10);
  _settings.setData(OSCColor, QString("red"));
  _settings.setData(OSCPlot, QString("Histogram Bar"));
  _settings.setData(OSCLabel, QString("ADOSC"));

  _methodList << "AD";
  _methodList << "ADOSC";
}

int AD::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  _settings.getData(Method, s);
  int method = _methodList.indexOf(s);

  switch (method)
  {
    case 1:
    {
      int fp = _settings.getInt(FastPeriod);
      int sp = _settings.getInt(SlowPeriod);
      
      _settings.getData(OSCColor, s);
      QColor color(s);

      _settings.getData(OSCPlot, s);
      PlotFactory fac;
      int lineType = fac.typeFromString(s);
      
      PlotLine *line = getADOSC(data, fp, sp, lineType, color);
      if (! line)
	return 1;
      
      _settings.getData(OSCLabel, s);
      line->setLabel(s);

      s = "0";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
    default:
    {
      _settings.getData(ADColor, s);
      QColor color(s);

      _settings.getData(ADPlot, s);
      PlotFactory fac;
      int lineType = fac.typeFromString(s);

      PlotLine *line = getAD(data, lineType, color);
      if (! line)
	return 1;
      
      _settings.getData(ADLabel, s);
      line->setLabel(s);

      s = "0";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
  }

  return 0;
}

int AD::getCUS (QStringList &set, Indicator &ind, BarData *data)
{
  QString s;
  int method = _methodList.indexOf(set[2]);

  int rc = 1;
  switch (method)
  {
    case 0:
      rc = getCUS_AD(set, ind, data);
      break;
    case 1:
      rc = getCUS_ADOSC(set, ind, data);
      break;
  }

  return rc;
}

int AD::getCUS_AD (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,AD,AD,<NAME>,<LINE TYPE>,<COLOR>
  //      0      1     2  3    4       5          6

  if (set.count() != 7)
  {
    qDebug() << _indicator << "::getCUS_AD: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS_AD: duplicate name" << set[4];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[5]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS_AD: invalid plot type" << set[5];
    return 1;
  }

  QColor color(set[6]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS_AD: invalid color" << set[6];
    return 1;
  }

  PlotLine *line = getAD(data, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);
  
  ind.setLine(set[4], line);

  return 0;
}

int AD::getCUS_ADOSC (QStringList &set, Indicator &ind, BarData *data)
{
  // INDICATOR,PLUGIN,AD,ADOSC,<NAME>,<FAST_PERIOD>,<SLOW_PERIOD>,<PLOT TYPE>,<COLOR>
  //      0       1    2    3    4         5             6             7         8

  if (set.count() != 9)
  {
    qDebug() << _indicator << "::getCUS_ADOSC: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS_ADOSC: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int fast = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS_ADOSC: invalid fast period" << set[5];
    return 1;
  }

  int slow = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << _indicator << "::getCUS_ADOSC: invalid slow period" << set[6];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[7]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS_ADOSC: invalid plot type" << set[7];
    return 1;
  }

  QColor color(set[8]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS_ADOSC: invalid color" << set[8];
    return 1;
  }

  PlotLine *line = getADOSC(data, fast, slow, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

PlotLine * AD::getAD (BarData *data, int lineType, QColor &color)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
    close[loop] = (TA_Real) bar->getClose();
    volume[loop] = (TA_Real) bar->getVolume();
  }

  TA_RetCode rc = TA_AD(0,
                        size - 1,
                        &high[0],
                        &low[0],
                        &close[0],
                        &volume[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getAD: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setData(dataLoop, new PlotLineBar(color, out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

PlotLine * AD::getADOSC (BarData *data, int fast, int slow, int lineType, QColor &color)
{
  int size = data->count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    high[loop] = (TA_Real) bar->getHigh();
    low[loop] = (TA_Real) bar->getLow();
    close[loop] = (TA_Real) bar->getClose();
    volume[loop] = (TA_Real) bar->getVolume();
  }

  TA_RetCode rc = TA_ADOSC(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           &volume[0],
                           fast,
                           slow,
                           &outBeg,
                           &outNb,
                           &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getADOSC: TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setData(dataLoop, new PlotLineBar(color, out[outLoop]));
    dataLoop--;
    outLoop--;
  }

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

  _settings.getData(Method, d);
  dialog->addComboItem((int) Method, page, QObject::tr("Method"), _methodList, d);

  page++;
  k = QObject::tr("AD");
  dialog->addPage(page, k);

  _settings.getData(ADColor, d);
  dialog->addColorItem((int) ADColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);
  
  _settings.getData(ADPlot, d);
  dialog->addComboItem((int) ADPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(ADLabel, d);
  dialog->addTextItem((int) ADLabel, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("OSC");
  dialog->addPage(page, k);

  _settings.getData(OSCColor, d);
  dialog->addColorItem(OSCColor, page, QObject::tr("Color"), d);

  _settings.getData(OSCPlot, d);
  dialog->addComboItem(OSCPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(OSCLabel, d);
  dialog->addTextItem(OSCLabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(FastPeriod, page, QObject::tr("Fast Period"), _settings.getInt(FastPeriod), 1, 100000);

  dialog->addIntItem(SlowPeriod, page, QObject::tr("Slow Period"), _settings.getInt(SlowPeriod), 1, 100000);

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


