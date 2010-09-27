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

#include "VOL.h"
#include "FunctionMA.h"
#include "VOLDialog.h"
#include "PlotOHLC.h"
#include "Curve.h"

#include <QtDebug>

VOL::VOL ()
{
  _indicator = "VOL";
}

int VOL::getIndicator (Indicator &ind, BarData &data)
{
  Setting set = ind.settings();
  
  QString s;
  set.getData(UpColor, s);
  QColor up(s);

  set.getData(DownColor, s);
  QColor down(s);

  set.getData(NeutralColor, s);
  QColor neutral(s);

  Curve *line = getVOL(up, down, neutral, data);
  if (! line)
    return 1;

  set.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  set.getData(Label, s);
  line->setLabel(s);

  line->setZ(0);
  
  ind.setLine(0, line);

  // vol ma
  int period = set.getInt(MAPeriod);

  set.getData(MAType, s);
  FunctionMA mau;
  int type = mau.typeFromString(s);

  Curve *ma = mau.calculate(line, period, type);
  if (ma)
  {
    set.getData(MAPlot, s);
    ma->setType((Curve::Type) ma->typeFromString(s));

    set.getData(MAColor, s);
    QColor c(s);
    ma->setColor(c);

    set.getData(MALabel, s);
    ma->setLabel(s);
    
    ma->setZ(1);
    
    ind.setLine(1, ma);
  }

  return 0;
}

Curve * VOL::getVOL (QColor &up, QColor &down, QColor &neutral, BarData &data)
{
  if (data.count() < 2)
    return 0;
  
  Curve *vol = new Curve;

  // set the first bar to neutral
  int loop = 0;
  Bar bar = data.getBar(loop);

  vol->setBar(loop, new CurveBar(neutral, bar.getVolume()));

  loop++;
  for (; loop < data.count(); loop++)
  {
    Bar bar = data.getBar(loop);
    Bar pbar = data.getBar(loop - 1);

    if (bar.getClose() < pbar.getClose())
      vol->setBar(loop, new CurveBar(down, bar.getVolume()));
    else
    {
      if (bar.getClose() > pbar.getClose())
        vol->setBar(loop, new CurveBar(up, bar.getVolume()));
      else
        vol->setBar(loop, new CurveBar(neutral, bar.getVolume()));
    }
  }

  return vol;
}

IndicatorPluginDialog * VOL::dialog (Indicator &i)
{
  return new VOLDialog(i);
}

void VOL::defaults (Indicator &i)
{
  Setting set;
  set.setData(UpColor, "green");
  set.setData(DownColor, "red");
  set.setData(NeutralColor, "blue");
  set.setData(Plot, "Histogram Bar");
  set.setData(Label, _indicator);
  set.setData(MAColor, "yellow");
  set.setData(MAPlot, "Line");
  set.setData(MALabel, "VOLMA");
  set.setData(MAPeriod, 10);
  set.setData(MAType, "SMA");
  i.setSettings(set);
}

void VOL::plotNames (Indicator &i, QStringList &l)
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
  VOL *o = new VOL;
  return ((IndicatorPlugin *) o);
}


