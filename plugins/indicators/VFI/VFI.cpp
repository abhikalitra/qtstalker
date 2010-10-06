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

#include "VFI.h"
#include "VFIDialog.h"
#include "Curve.h"

#include <QtDebug>
#include <cmath>

VFI::VFI ()
{
  _indicator = "VFI";
}

int VFI::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  int period = settings.getInt(_Period);

  Curve *line = calculate(period, data);
  if (! line)
    return 1;

  QString s;
  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(0);
  ind.setLine(s, line);

  return 0;
}

int VFI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,VFI,<NAME>,<PERIOD>
  //     0       1     2    3        4

  if (set.count() != 5)
  {
    qDebug() << "VFI::getCUS: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "VFI::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "VFI::getCUS: invalid period" << set[4];
    return 1;
  }

  Curve *line = calculate(period, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * VFI::dialog (Indicator &i)
{
  return new VFIDialog(i);
}

void VFI::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_Period, 100);
  i.setSettings(set);
}

void VFI::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * VFI::calculate (int period, BarData &data)
{
  int size = data.count();

  if (size < period)
    return 0;

  Curve *vfi = new Curve;

  int loop = period;
  for (; loop < size; loop++)
  {
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    Bar bar = data.getBar(loop - period);
    double close = bar.getClose();
    double high = bar.getHigh();
    double low = bar.getLow();
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      bar = data.getBar(i);
      double ytypical = typical;
      close = bar.getClose();
      high = bar.getHigh();
      low = bar.getLow();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += bar.getVolume();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    bar = data.getBar(loop - period);
    close = bar.getClose();
    high = bar.getHigh();
    low = bar.getLow();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      bar = data.getBar(i);
      double ytypical = typical;
      double volume = bar.getVolume();
      close = bar.getClose();
      high = bar.getHigh();
      low = bar.getLow();
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    vfi->setBar(loop, new CurveBar(t));
  }

  return vfi;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VFI *o = new VFI;
  return ((IndicatorPlugin *) o);
}

