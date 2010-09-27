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

#include "WILLR.h"
#include "WILLRDialog.h"
#include "FunctionWILLR.h"
#include "Curve.h"

#include <QtDebug>

WILLR::WILLR ()
{
  _indicator = "WILLR";
}

int WILLR::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  int period = settings.getInt(Period);

  FunctionWILLR f;
  Curve *line = f.calculate(period, data);
  if (! line)
    return 1;

  QString s;
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

int WILLR::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionWILLR f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * WILLR::dialog (Indicator &i)
{
  return new WILLRDialog(i);
}

void WILLR::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Period, 14);
  i.setSettings(set);
}

void WILLR::plotNames (Indicator &i, QStringList &l)
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
  WILLR *o = new WILLR;
  return ((IndicatorPlugin *) o);
}

