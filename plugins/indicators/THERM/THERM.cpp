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
#include "THERMDialog.h"
#include "FunctionTHERM.h"
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
  int smoothing = settings.getInt(Smoothing);

  FunctionMA mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  FunctionTHERM f;
  Curve *line = f.calculate(smoothing, type, data);
  if (! line)
    return 1;

  line->setType(Curve::HistogramBar);

  settings.getData(MAColor, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(Label, s);
  line->setLabel(s);

  // therm ma
  int maPeriod = settings.getInt(MAPeriod);

  settings.getData(MAType, s);
  int maType = mau.typeFromString(s);

  Curve *ma = mau.calculate(line, maPeriod, maType);
  if (! ma)
  {
    delete line;
    return 1;
  }

  settings.getData(MAPlot, s);
  ma->setType((Curve::Type) ma->typeFromString(s));

  settings.getData(MAColor, s);
  c.setNamedColor(s);
  ma->setColor(c);

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

  line->setZ(0);
  ind.setLine(0, line);

  ma->setZ(1);
  ind.setLine(1, ma);

  return 0;
}

int THERM::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionTHERM f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * THERM::dialog (Indicator &i)
{
  return new THERMDialog(i);
}

void THERM::defaults (Indicator &i)
{
  Setting set;
  set.setData(DownColor, "green");
  set.setData(UpColor, "magenta");
  set.setData(ThreshColor, "red");
  set.setData(MAColor, "yellow");
  set.setData(MAPlot, "Line");
  set.setData(Label, _indicator);
  set.setData(MALabel, "THERM_MA");
  set.setData(Threshold, 3);
  set.setData(Smoothing, 2);
  set.setData(MAPeriod, 22);
  set.setData(MAType, "SMA");
  set.setData(SmoothingType, "SMA");
  i.setSettings(set);
}

void THERM::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(Label, s);
  l.append(s);

  settings.getData(MALabel, s);
  l.append(s);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  THERM *o = new THERM;
  return ((IndicatorPlugin *) o);
}

