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

#include "RSI.h"
#include "FunctionMA.h"
#include "FunctionRSI.h"
#include "RSIDialog.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

RSI::RSI ()
{
  _indicator = "RSI";
}

int RSI::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();

  // ref1 line
  Curve *line = new Curve;
  line->setType((Curve::Horizontal));

  QString s;
  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // ref2 line
  line = new Curve;
  line->setType((Curve::Horizontal));

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref2)));

  line->setZ(0);
  ind.setLine(1, line);

  settings.getData(Input, s);
  Curve *in = g_barData.getInput(g_barData.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);
  int smoothing = settings.getInt(Smoothing);

  FunctionMA mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  FunctionRSI r;
  line = r.calculate(in, period, smoothing, type);
  if (! line)
  {
    delete in;
    return 1;
  }

  settings.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(Color, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(Label, s);
  line->setLabel(s);

  line->setZ(2);
  ind.setLine(2, line);

  delete in;

  return 0;
}

int RSI::getCUS (QStringList &set, Indicator &ind)
{
  FunctionRSI f;
  return f.script(set, ind);
}

IndicatorPluginDialog * RSI::dialog (Indicator &i)
{
  return new RSIDialog(i);
}

void RSI::defaults (Indicator &i)
{
  Setting set;
  set.setData(RSI::Color, "red");
  set.setData(RSI::Plot, "Line");
  set.setData(RSI::Label, _indicator);
  set.setData(RSI::Period, 14);
  set.setData(RSI::Smoothing, 1);
  set.setData(RSI::SmoothingType, "SMA");
  set.setData(RSI::Input, "Close");
  set.setData(RSI::Ref1Color, "white");
  set.setData(RSI::Ref2Color, "white");
  set.setData(RSI::Ref1, 30);
  set.setData(RSI::Ref2, 70);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  RSI *o = new RSI;
  return ((IndicatorPlugin *) o);
}


