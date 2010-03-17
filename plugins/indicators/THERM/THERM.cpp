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

/* The "Market Thermometer" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/


#include "THERM.h"
#include "MATH1.h"

#include <QtDebug>
#include <cmath>


THERM::THERM ()
{
  indicator = "THERM";

  settings.setData(DownColor, "green");
  settings.setData(UpColor, "magenta");
  settings.setData(ThreshColor, "red");
  settings.setData(MAColor, "yellow");
  settings.setData(MAPlot, "Line");
  settings.setData(Label, indicator);
  settings.setData(MALabel, "THERM_MA");
  settings.setData(Threshold, 3);
  settings.setData(Smoothing, 2);
  settings.setData(MAPeriod, 22);
  settings.setData(MAType, "SMA");
  settings.setData(SmoothingType, "SMA");
}

int THERM::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  int smoothing = settings.getInt(Smoothing);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  settings.getData(SmoothingType, s);
  int type = maList.indexOf(s);

  PlotLine *line = getTHERM(data, smoothing, type);
  if (! line)
    return 1;

  s = "HistogramBar";
  line->setPlugin(s);
  settings.getData(Label, s);
  line->setLabel(s);

  // therm ma
  int maPeriod = settings.getInt(MAPeriod);

  settings.getData(MAType, s);
  int maType = maList.indexOf(s);

  PlotLine *ma = m.getMA(line, maPeriod, maType);
  if (! ma)
  {
    delete line;
    return 1;
  }

  settings.getData(MAColor, s);
  ma->setColor(s);
  settings.getData(MAPlot, s);
  ma->setPlugin(s);
  settings.getData(MALabel, s);
  ma->setLabel(s);

  // assign therm colors
  int thermLoop = line->count() - 1;
  int maLoop = ma->count() - 1;
  double threshold = settings.getDouble(Threshold);

  settings.getData(ThreshColor, s);
  QColor threshColor(s);
  settings.getData(UpColor, s);
  QColor upColor(s);
  settings.getData(DownColor, s);
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

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
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
    MATH1 m;
    PlotLine *ma = m.getMA(line, smoothing, type);
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

int THERM::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up Color"), d);

  settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down Color"), d);

  settings.getData(ThreshColor, d);
  dialog->addColorItem(ThreshColor, page, QObject::tr("Threshold Color"), d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  dialog->addDoubleItem(Threshold, page, QObject::tr("Threshold"), settings.getDouble(Threshold), 0, 100000);

  dialog->addIntItem(Smoothing, page, QObject::tr("Smoothing"), settings.getInt(Smoothing), 1, 100000);

  MATH1 m;
  QStringList maList;
  m.getMAList(maList);
  
  settings.getData(SmoothingType, d);
  dialog->addComboItem(SmoothingType, page, QObject::tr("Smoothing Type"), maList, d);

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), settings.getInt(MAPeriod), 1, 100000);

  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("MA Type"), maList, d);

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
  THERM *o = new THERM;
  return ((IndicatorPlugin *) o);
}

