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

/* The "SafeZone Stop" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.173 */

#include "SZ.h"
#include "FunctionBARS.h"
#include "SZDialog.h"
#include "FunctionSZ.h"
#include "Curve.h"

#include <QtDebug>
#include <cmath>

SZ::SZ ()
{
  _indicator = "SZ";
}

int SZ::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(Method, s);

  FunctionSZ f;
  QStringList methodList = f.list();
  int method = methodList.indexOf(s);

  int period = settings.getInt(Period);
  int ndperiod = settings.getInt(NoDeclinePeriod);
  double coeff = settings.getDouble(Coefficient);

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  FunctionBARS b;
  Curve *bars = b.getBARS(up, down, neutral, data);
  if (bars)
  {
    bars->setZ(0);
    ind.setLine(0, bars);
  }

  Curve *line = f.calculate(method, period, ndperiod, coeff, data);
  if (! line)
    return 1;

  settings.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(Label, s);
  line->setLabel(s);
  
  line->setZ(1);
  ind.setLine(1, line);

  return 0;
}

int SZ::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionSZ f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * SZ::dialog (Indicator &i)
{
  return new SZDialog(i);
}

void SZ::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Period, 10);
  set.setData(Method, "Long");
  set.setData(NoDeclinePeriod, 2);
  set.setData(Coefficient, 2);
  i.setSettings(set);
}

void SZ::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(Label, s);
  l.append(s);
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  SZ *o = new SZ;
  return ((IndicatorPlugin *) o);
}

