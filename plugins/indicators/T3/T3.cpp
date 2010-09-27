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

#include "T3.h"
#include "FunctionT3.h"
#include "FunctionBARS.h"
#include "T3Dialog.h"
#include "Curve.h"

#include <QtDebug>

T3::T3 ()
{
  _indicator = "T3";
}

int T3::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

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

  QString s;
  settings.getData(Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);
  double vfactor = settings.getDouble(VFactor);

  FunctionT3 f;
  Curve *line = f.calculate(in, period, vfactor);
  if (! line)
  {
    delete in;
    return 1;
  }

  settings.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(Label, s);
  line->setLabel(s);
  
  line->setZ(1);
  ind.setLine(1, line);

  delete in;

  return 0;
}

int T3::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionT3 f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * T3::dialog (Indicator &i)
{
  return new T3Dialog(i);
}

void T3::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Period, 5);
  set.setData(Input, "Close");
  set.setData(VFactor, 0.7);
  i.setSettings(set);
}

void T3::plotNames (Indicator &i, QStringList &l)
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
  T3 *o = new T3;
  return ((IndicatorPlugin *) o);
}

