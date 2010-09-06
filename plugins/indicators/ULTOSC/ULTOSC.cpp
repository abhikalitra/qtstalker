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

#include "ULTOSC.h"
#include "FunctionULTOSC.h"
#include "ULTOSCDialog.h"
#include "Curve.h"

#include <QtDebug>

ULTOSC::ULTOSC ()
{
  _indicator = "ULTOSC";
}

int ULTOSC::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // 30 ref line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, settings.getDouble(Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // 50 ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, settings.getDouble(Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  // 70 ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref3Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, settings.getDouble(Ref3)));
  
  line->setZ(2);
  ind.setLine(2, line);

  // ultosc line
  int sp = settings.getInt(ShortPeriod);
  int mp = settings.getInt(MidPeriod);
  int lp = settings.getInt(LongPeriod);

  FunctionULTOSC f;
  line = f.calculate(data, sp, mp, lp);
  if (! line)
    return 1;

  settings.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(Color, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(Label, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(3, line);

  return 0;
}

int ULTOSC::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionULTOSC f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * ULTOSC::dialog (Indicator &i)
{
  return new ULTOSCDialog(i);
}

void ULTOSC::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(ShortPeriod, 7);
  set.setData(MidPeriod, 14);
  set.setData(LongPeriod, 28);
  set.setData(Ref1Color, "white");
  set.setData(Ref2Color, "white");
  set.setData(Ref3Color, "white");
  set.setData(Ref1, 30);
  set.setData(Ref2, 50);
  set.setData(Ref3, 70);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ULTOSC *o = new ULTOSC;
  return ((IndicatorPlugin *) o);
}

