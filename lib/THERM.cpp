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

/* The "Market Thermometer" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/


#include "THERM.h"

#include <QtDebug>
#include <math.h>


THERM::THERM ()
{
  indicator = "THERM";
  downColorKey = QObject::tr("Down Color");
  upColorKey = QObject::tr("Up Color");
  threshColorKey = QObject::tr("Threshold Color");
  threshKey = QObject::tr("Threshold");
  smoothKey = QObject::tr("Smoothing Period");
  smoothTypeKey = QObject::tr("Smoothing Type");

  QString d;
  d = "green";
  settings.setData(downColorKey, d);

  d = "magenta";
  settings.setData(upColorKey, d);

  d = "red";
  settings.setData(threshColorKey, d);

  d = "yellow";
  settings.setData(maColorKey, d);

  d = "Line";
  settings.setData(maPlotKey, d);

  settings.setData(labelKey, indicator);

  d = "THERM_MA";
  settings.setData(maLabelKey, d);

  settings.setData(threshKey, 3);
  settings.setData(smoothKey, 2);
  settings.setData(maPeriodKey, 22);

  d = "SMA";
  settings.setData(maTypeKey, d);
  settings.setData(smoothTypeKey, d);
}

int THERM::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int smoothing = settings.getInt(smoothKey);

  settings.getData(smoothTypeKey, s);
  int type = maList.indexOf(s);

  PlotLine *line = getTHERM(data, smoothing, type);
  if (! line)
    return 1;

  // therm ma
  int maPeriod = settings.getInt(maPeriodKey);

  settings.getData(maTypeKey, s);
  int maType = maList.indexOf(s);

  PlotLine *ma = getMA(line, maPeriod, maType);
  if (! ma)
  {
    delete line;
    return 1;
  }

  settings.getData(maColorKey, s);
  ma->setColor(s);
  settings.getData(maPlotKey, s);
  ma->setType(s);
  settings.getData(maLabelKey, s);
  ma->setLabel(s);

  // assign therm colors
  line->setColorFlag(TRUE);
  line->setType(PlotLine::HistogramBar);
  int thermLoop = line->getSize() - 1;
  int maLoop = ma->getSize() - 1;
  double threshold = settings.getDouble(threshKey);

  settings.getData(threshColorKey, s);
  QColor threshColor(s);

  settings.getData(upColorKey, s);
  QColor upColor(s);

  settings.getData(downColorKey, s);
  QColor downColor(s);

  while (thermLoop > -1)
  {
    if (maLoop > -1)
    {
      double thrm = line->getData(thermLoop);
      double thrmma = ma->getData(maLoop);

      if (thrm > (thrmma * threshold))
        line->setColorBar(thermLoop, threshColor);
      else
      {
        if (thrm > thrmma)
          line->setColorBar(thermLoop, upColor);
        else
          line->setColorBar(thermLoop, downColor);
      }
    }
    else
      line->setColorBar(thermLoop, downColor);

    thermLoop--;
    maLoop--;
  }

  settings.getData(plotKey, s);
  line->setType(s);

  settings.getData(labelKey, s);
  line->setLabel(s);

  ind.addLine(line);

  ind.addLine(ma);

  return 0;
}

int THERM::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,THERM,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>

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

  bool ok;
  int smoothing = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid smoothing" << set[3];
    return 1;
  }

  int type = maList.indexOf(set[4]);
  if (type == -1)
  {
    qDebug() << indicator << "::calculate: invalid smoothing type" << set[4];
    return 1;
  }

  PlotLine *line = getTHERM(data, smoothing, type);
  if (! line)
    return 1;

  tlines.insert(set[2], line);

  return 0;
}

PlotLine * THERM::getTHERM (BarData *data, int smoothing, int type)
{
  PlotLine *line = new PlotLine();
  int loop;
  double thermometer = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = fabs(data->getHigh(loop) - data->getHigh(loop - 1));
    double lo = fabs(data->getLow(loop - 1) - data->getLow(loop));

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    line->append(thermometer);
  }

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(line, smoothing, type);
    if (! ma)
    {
      delete line;
      return 0;
    }

    delete line;
    line = ma;
  }

  return line;
}

int THERM::dialog ()
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(upColorKey, d);
  dialog->addColorItem(page, upColorKey, d);

  settings.getData(downColorKey, d);
  dialog->addColorItem(page, downColorKey, d);

  settings.getData(threshColorKey, d);
  dialog->addColorItem(page, threshColorKey, d);

  settings.getData(labelKey, d);
  dialog->addTextItem(page, labelKey, d);

  dialog->addDoubleItem(page, threshKey, settings.getDouble(threshKey), 0, 100000);

  dialog->addIntItem(page, smoothKey, settings.getInt(smoothKey), 1, 100000);

  settings.getData(smoothTypeKey, d);
  dialog->addComboItem(page, smoothTypeKey, maList, d);

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  settings.getData(maColorKey, d);
  dialog->addColorItem(page, maColorKey, d);

  settings.getData(maPlotKey, d);
  dialog->addComboItem(page, maPlotKey, plotList, d);

  settings.getData(maLabelKey, d);
  dialog->addTextItem(page, maLabelKey, d);

  dialog->addIntItem(page, maPeriodKey, settings.getInt(maPeriodKey), 1, 100000);

  settings.getData(maTypeKey, d);
  dialog->addComboItem(page, maTypeKey, maList, d);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  QStringList keys;
  settings.getKeyList(keys);
  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString d;
    dialog->getItem(keys[loop], d);
    if (! d.isEmpty())
      settings.setData(keys[loop], d);
  }

  delete dialog;
  return rc;
}

