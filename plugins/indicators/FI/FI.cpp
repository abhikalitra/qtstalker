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

#include "FI.h"
#include "FunctionMA.h"
#include "FIDialog.h"
#include "Curve.h"

#include <QtDebug>

FI::FI ()
{
  _indicator = "FI";
}

int FI::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  int period = settings.getInt(_Period);

  FunctionMA mau;
  settings.getData(_MAType, s);
  int ma = mau.typeFromString(s);

  Curve *line = calculate(period, ma, data);
  if (! line)
    return 1;

  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);

  line->setZ(0);
  ind.setLine(0, line);

  return 0;
}

int FI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,FI,<NAME>,<PERIOD>,<MA_TYPE>
  //     0       1    2     3      4         5

  if (set.count() != 6)
  {
    qDebug() << "FI::getCUS: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FI::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FI::getCUS: invalid period" << set[4];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[5]);
  if (ma == -1)
  {
    qDebug() << "FI::getCUS: invalid ma" << set[5];
    return 1;
  }

  Curve *line = calculate(period, ma, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * FI::dialog (Indicator &i)
{
  return new FIDialog(i);
}

void FI::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Histogram Bar");
  set.setData(_Label, _indicator);
  set.setData(_MAType, "EMA");
  set.setData(_Period, 2);
  i.setSettings(set);
}

void FI::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * FI::calculate (int period, int type, BarData &data)
{
  if (data.count() < period)
    return 0;

  Curve *line = new Curve;

  int loop = 1;
  double force = 0;
  for (; loop < data.count(); loop++)
  {
    Bar bar = data.getBar(loop);
    Bar pbar = data.getBar(loop - 1);
    double cdiff = bar.getClose() - pbar.getClose();
    force = bar.getVolume() * cdiff;

    line->setBar(loop, new CurveBar(force));
  }

  if (period > 1)
  {
    FunctionMA mau;
    Curve *ma = mau.calculate(line, period, type);
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
  FI *o = new FI;
  return ((IndicatorPlugin *) o);
}


