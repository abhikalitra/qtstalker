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

#include "ATR.h"
#include "Curve.h"
#include "FunctionATR.h"
#include "ATRDialog.h"

#include <QtDebug>

ATR::ATR ()
{
  _indicator = "ATR";
}

int ATR::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  int period = settings.getInt(Period);

  FunctionATR f;
  QStringList l = f.list();
  
  QString s;
  settings.getData(Method, s);
  int method = l.indexOf(s);

  Curve *line = f.calculate(data, period, method);
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

int ATR::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionATR f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * ATR::dialog (Indicator &i)
{
  return new ATRDialog(i);
}

void ATR::defaults (Indicator &i)
{
  Setting set;
  set.setData(Method, "ATR");
  set.setData(Period, 14);
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ATR *o = new ATR;
  return ((IndicatorPlugin *) o);
}


