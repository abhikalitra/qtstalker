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
#include "MAFactory.h"
#include "PlotFactory.h"

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

  MAFactory mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  settings.getData(MAColor, s);
  QColor color(s);

  PlotFactory fac;
  s = "Histogram Bar";
  int lineType = fac.typeFromString(s);

  PlotLine *line = getTHERM(data, smoothing, type, lineType, color);
  if (! line)
    return 1;

  settings.getData(Label, s);
  line->setLabel(s);

  // therm ma
  int maPeriod = settings.getInt(MAPeriod);

  settings.getData(MAType, s);
  int maType = mau.typeFromString(s);

  settings.getData(MAColor, s);
  color.setNamedColor(s);

  settings.getData(MAPlot, s);
  lineType = fac.typeFromString(s);

  PlotLine *ma = mau.ma(line, maPeriod, maType, lineType, color);
  if (! ma)
  {
    delete line;
    return 1;
  }

  settings.getData(MALabel, s);
  ma->setLabel(s);

  // assign therm colors
  double threshold = settings.getDouble(Threshold);

  settings.getData(ThreshColor, s);
  QColor threshColor(s);
  settings.getData(UpColor, s);
  QColor upColor(s);
  settings.getData(DownColor, s);
  QColor downColor(s);

  QList<int> keys;
  line->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *lbar = line->data(keys.at(loop));
    PlotLineBar *mbar = ma->data(keys.at(loop));
    if (! mbar)
      lbar->setColor(downColor);
    else
    {
      double thrm = lbar->data();
      double thrmma = mbar->data();

      if (thrm > (thrmma * threshold))
        lbar->setColor(threshColor);
      else
      {
        if (thrm > thrmma)
          lbar->setColor(upColor);
        else
          lbar->setColor(downColor);
      }
    }
  }

  ind.addLine(line);
  ind.addLine(ma);

  return 0;
}

int THERM::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,THERM,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>,<PLOT TYPE>,<COLOR>

  if (set.count() != 8)
  {
    qDebug() << indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing" << set[4];
    return 1;
  }

  MAFactory mau;
  int type = mau.typeFromString(set[5]);
  if (type == -1)
  {
    qDebug() << indicator << "::getCUS: invalid smoothing type" << set[5];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getTHERM(data, smoothing, type, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * THERM::getTHERM (BarData *data, int smoothing, int type, int lineType, QColor &color)
{
  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int loop = 1;
  double thermometer = 0;
  for (; loop < (int) data->count(); loop++)
  {
    Bar *bar = data->getBar(loop);
    Bar *pbar = data->getBar(loop - 1);
    double high = fabs(bar->getHigh() - pbar->getHigh());
    double lo = fabs(pbar->getLow() - bar->getLow());

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    line->setData(loop, new PlotLineBar(color, thermometer));
  }

  if (smoothing > 1)
  {
    MAFactory mau;
    PlotLine *ma = mau.ma(line, smoothing, type, lineType, color);
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

  QStringList maList;
  MAFactory mau;
  mau.list(maList);
  
  settings.getData(SmoothingType, d);
  dialog->addComboItem(SmoothingType, page, QObject::tr("Smoothing Type"), maList, d);

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

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

