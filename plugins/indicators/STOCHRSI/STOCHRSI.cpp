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

#include "STOCHRSI.h"
#include "FunctionMA.h"
#include "FunctionSTOCHRSI.h"
#include "STOCHRSIDialog.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

STOCHRSI::STOCHRSI ()
{
  _indicator = "STOCHRSI";
}

int STOCHRSI::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();

  // create first ref line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // create second ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  settings.getData(Input, s);
  Curve *in = g_barData.getInput(g_barData.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  FunctionSTOCHRSI f;
  line = f.calculate(in, period);
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

int STOCHRSI::getCUS (QStringList &set, Indicator &ind)
{
  FunctionSTOCHRSI f;
  return f.script(set, ind);
}

IndicatorPluginDialog * STOCHRSI::dialog (Indicator &i)
{
  return new STOCHRSIDialog(i);
}

void STOCHRSI::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Ref1Color, "white");
  set.setData(Ref2Color, "white");
  set.setData(Plot, "Line");
  set.setData(Label, "STOCHRSI");
  set.setData(Ref1, 0.2);
  set.setData(Ref2, 0.8);
  set.setData(Input, "Close");
  set.setData(Period, 14);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCHRSI *o = new STOCHRSI;
  return ((IndicatorPlugin *) o);
}


