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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/

#include "THERM.h"
#include "FunctionMA.h"
#include "Curve.h"

#include <QtDebug>
#include <cmath>

THERM::THERM ()
{
  _indicator = "THERM";
}

int THERM::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  int smoothing = settings.getInt(_Smoothing);

  FunctionMA mau;
  settings.getData(_SmoothingType, s);
  int type = mau.typeFromString(s);

  Curve *line = calculate(smoothing, type, data);
  if (! line)
    return 1;

  line->setType(Curve::HistogramBar);

  settings.getData(_MAColor, s);
  QColor c(s);
  line->setColor(c);

  // therm ma
  int maPeriod = settings.getInt(_MAPeriod);

  settings.getData(_MAType, s);
  int maType = mau.typeFromString(s);

  Curve *ma = mau.calculate(line, maPeriod, maType);
  if (! ma)
  {
    delete line;
    return 1;
  }

  settings.getData(_MAPlot, s);
  ma->setType((Curve::Type) ma->typeFromString(s));

  settings.getData(_MAColor, s);
  c.setNamedColor(s);
  ma->setColor(c);

  // assign therm colors
  double threshold = settings.getDouble(_Threshold);

  settings.getData(_ThreshColor, s);
  QColor threshColor(s);
  
  settings.getData(_UpColor, s);
  QColor upColor(s);
  
  settings.getData(_DownColor, s);
  QColor downColor(s);

  QList<int> keys;
  line->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *lbar = line->bar(keys.at(loop));
    CurveBar *mbar = ma->bar(keys.at(loop));
    
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

  settings.getData(_Label, s);
  line->setLabel(s);

  line->setZ(0);
  ind.setLine(s, line);

  settings.getData(_MALabel, s);
  ma->setLabel(s);

  ma->setZ(1);
  ind.setLine(s, ma);

  return 0;
}

int THERM::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,THERM,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2      3             4               5

  if (set.count() != 6)
  {
    qDebug() << "THERM::getCUS: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "THERM::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "THERM::getCUS: invalid smoothing" << set[4];
    return 1;
  }

  FunctionMA mau;
  int type = mau.typeFromString(set[5]);
  if (type == -1)
  {
    qDebug() << "THERM::getCUS: invalid smoothing type" << set[5];
    return 1;
  }

  Curve *line = calculate(smoothing, type, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * THERM::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("THERM.html");

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_UpColor, d);
  dialog->addColor(tab, _UpColor, tr("Up Color"), d);

  _settings.getData(_DownColor, d);
  dialog->addColor(tab, _DownColor, tr("Down Color"), d);

  _settings.getData(_ThreshColor, d);
  dialog->addColor(tab, _ThreshColor, tr("Threshold Color"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  dialog->addDouble(tab, _Threshold, tr("Threshold"), _settings.getDouble(_Threshold), 100000, 0);

  dialog->addInt(tab, _Smoothing, tr("Smoothing"), _settings.getInt(_Smoothing), 100000, 1);

  _settings.getData(_SmoothingType, d);
  dialog->addMA(tab, _SmoothingType, tr("Smoothing Type"), d);

  // MA tab
  tab = dialog->addTab("MA");

  _settings.getData(_MAColor, d);
  dialog->addColor(tab, _MAColor, tr("Color"), d);

  _settings.getData(_MAPlot, d);
  dialog->addPlot(tab, _MAPlot, tr("Plot"), d);

  _settings.getData(_MALabel, d);
  dialog->addText(tab, _MALabel, tr("Label"), d);

  dialog->addInt(tab, _MAPeriod, tr("Period"), _settings.getInt(_MAPeriod), 100000, 1);

  _settings.getData(_MAType, d);
  dialog->addMA(tab, _MAType, tr("MA Type"), d);

  return dialog;
}

void THERM::defaults (Indicator &i)
{
  Setting set;
  set.setData(_DownColor, "green");
  set.setData(_UpColor, "magenta");
  set.setData(_ThreshColor, "red");
  set.setData(_MAColor, "yellow");
  set.setData(_MAPlot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_MALabel, "THERM_MA");
  set.setData(_Threshold, 3);
  set.setData(_Smoothing, 2);
  set.setData(_MAPeriod, 22);
  set.setData(_MAType, "SMA");
  set.setData(_SmoothingType, "SMA");
  i.setSettings(set);
}

void THERM::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_Label, s);
  l.append(s);

  settings.getData(_MALabel, s);
  l.append(s);
}

Curve * THERM::calculate (int smoothing, int type, BarData &data)
{
  if (data.count() < smoothing || data.count() < 2)
    return 0;

  Curve *line = new Curve;

  int loop = 1;
  double thermometer = 0;
  for (; loop < (int) data.count(); loop++)
  {
    Bar bar = data.getBar(loop);
    Bar pbar = data.getBar(loop - 1);
    double high = fabs(bar.getHigh() - pbar.getHigh());
    double lo = fabs(pbar.getLow() - bar.getLow());

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    line->setBar(loop, new CurveBar(thermometer));
  }

  if (smoothing > 1)
  {
    FunctionMA mau;
    Curve *ma = mau.calculate(line, smoothing, type);
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

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  THERM *o = new THERM;
  return ((IndicatorPlugin *) o);
}

