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

#include "VAR.h"
#include "VARDialog.h"
#include "FunctionVAR.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

VAR::VAR ()
{
  _indicator = "VAR";
}

int VAR::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(Input, s);
  Curve *in = g_barData.getInput(g_barData.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);

  FunctionVAR f;
  Curve *line = f.calculate(in, period);
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
  
  line->setZ(0);
  ind.setLine(0, line);

  delete in;

  return 0;
}

int VAR::getCUS (QStringList &set, Indicator &ind)
{
  FunctionVAR f;
  return f.script(set, ind);
}

IndicatorPluginDialog * VAR::dialog (Indicator &i)
{
  return new VARDialog(i);
}

void VAR::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Period, 5);
  set.setData(Input, "Close");
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  VAR *o = new VAR;
  return ((IndicatorPlugin *) o);
}

