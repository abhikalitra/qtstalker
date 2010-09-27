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
#include "FunctionFI.h"
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
  int period = settings.getInt(Period);

  FunctionMA mau;
  settings.getData(MAType, s);
  int ma = mau.typeFromString(s);

  FunctionFI f;
  Curve *line = f.calculate(period, ma, data);
  if (! line)
    return 1;

  settings.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(Label, s);
  line->setLabel(s);

  line->setZ(0);
  ind.setLine(0, line);

  return 0;
}

int FI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionFI f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * FI::dialog (Indicator &i)
{
  return new FIDialog(i);
}

void FI::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Histogram Bar");
  set.setData(Label, _indicator);
  set.setData(MAType, "EMA");
  set.setData(Period, 2);
  i.setSettings(set);
}

void FI::plotNames (Indicator &i, QStringList &l)
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
  FI *o = new FI;
  return ((IndicatorPlugin *) o);
}


