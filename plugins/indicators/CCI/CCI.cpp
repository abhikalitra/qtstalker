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

#include "CCI.h"
#include "FunctionMA.h"
#include "FunctionCCI.h"
#include "CCIDialog.h"
#include "Curve.h"

#include <QtDebug>
#include <cmath>

CCI::CCI ()
{
  _indicator = "CCI";
}

int CCI::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // create the ref1 line
  QString s;
  Curve *rline = new Curve;
  rline->setType(Curve::Horizontal);

  settings.getData(Ref1Color, s);
  QColor color(s);
  int ref = settings.getInt(Ref1);
  rline->setBar(0, new CurveBar(color, (double) ref));
  
  rline->setZ(0);
  ind.setLine(0, rline);

  // create the ref2 line
  rline = new Curve;
  rline->setType(Curve::Horizontal);

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);
  ref = settings.getInt(Ref2);
  rline->setBar(0, new CurveBar(color, (double) ref));

  rline->setZ(1);
  ind.setLine(1, rline);

  // get the CCI plot
  int period = settings.getInt(Period);
  int smoothing = settings.getInt(Smoothing);

  FunctionMA mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  FunctionCCI f;
  Curve *line = f.calculate(period, smoothing, type, data);
  if (! line)
    return 1;

  settings.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(Color, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(Label, s);
  line->setLabel(s);

  line->setZ(2);
  ind.setLine(2, line);

  return 0;
}

int CCI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionCCI f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * CCI::dialog (Indicator &i)
{
  return new CCIDialog(i);
}

void CCI::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Smoothing, 10);
  set.setData(SmoothingType, "SMA");
  set.setData(Period, 20);
  set.setData(Ref1, 100);
  set.setData(Ref2, -100);
  set.setData(Ref1Color, "white");
  set.setData(Ref2Color, "white");
  i.setSettings(set);
}

void CCI::plotNames (Indicator &i, QStringList &l)
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
  CCI *o = new CCI;
  return ((IndicatorPlugin *) o);
}


