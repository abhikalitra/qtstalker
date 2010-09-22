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

#include "MA.h"
#include "FunctionMA.h"
#include "FunctionBARS.h"
#include "MADialog.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>
#include <cmath>

MA::MA ()
{
  _indicator = "MA";
}

int MA::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  FunctionBARS b;
  Curve *bars = b.getBARS(up, down, neutral);
  if (bars)
  {
    bars->setZ(0);
    ind.setLine(0, bars);
  }

  QString s;
  settings.getData(Input, s);
  Curve *in = g_barData.getInput(g_barData.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  FunctionMA f;
  settings.getData(Method, s);
  FunctionMA mau;
  int method = f.typeFromString(s);

  Curve *line = f.calculate(in, period, method);
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

int MA::getCUS (QStringList &set, Indicator &ind)
{
  FunctionMA f;
  return f.script(set, ind);
}

IndicatorPluginDialog * MA::dialog (Indicator &i)
{
  return new MADialog(i);
}

void MA::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Input, "Close");
  set.setData(Period, 14);
  set.setData(Method, "SMA");
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MA *o = new MA;
  return ((IndicatorPlugin *) o);
}


