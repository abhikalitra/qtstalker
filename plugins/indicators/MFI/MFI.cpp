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

#include "MFI.h"
#include "FunctionMA.h"
#include "MFIDialog.h"
#include "FunctionMFI.h"
#include "Curve.h"

#include <QtDebug>

MFI::MFI ()
{
  _indicator = "MFI";
}

int MFI::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // Ref1 line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref1Color, s);
  QColor color(s);
  
  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // Ref2 line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  // mfi plot
  int period = settings.getInt(Period);
  int smoothing = settings.getInt(Smoothing);

  FunctionMA mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  FunctionMFI f;
  line = f.calculate(period, smoothing, type, data);
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

int MFI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionMFI f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * MFI::dialog (Indicator &i)
{
  return new MFIDialog(i);
}

void MFI::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Period, 14);
  set.setData(Smoothing, 10);
  set.setData(SmoothingType, "SMA");
  set.setData(Ref1Color, "white");
  set.setData(Ref2Color, "white");
  set.setData(Ref1, 20);
  set.setData(Ref2, 80);
  i.setSettings(set);
}

void MFI::plotNames (Indicator &i, QStringList &l)
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
  MFI *o = new MFI;
  return ((IndicatorPlugin *) o);
}


