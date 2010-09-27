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

#include "SAR.h"
#include "FunctionSAR.h"
#include "FunctionBARS.h"
#include "SARDialog.h"
#include "Curve.h"

#include <QtDebug>

SAR::SAR ()
{
  _indicator = "SAR";
}

int SAR::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  double tinit = settings.getDouble(Init);
  double tmax = settings.getDouble(Max);

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  FunctionBARS b;
  Curve *bars = b.getBARS(up, down, neutral, data);
  if (bars)
  {
    bars->setZ(0);
    ind.setLine(0, bars);
  }

  FunctionSAR f;
  Curve *line = f.calculate(tinit, tmax, data);
  if (! line)
    return 1;

  line->setType((Curve::Dot));
  
  QString s;
  settings.getData(Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(Label, s);
  line->setLabel(s);
  
  line->setZ(1);
  ind.setLine(1, line);

  return 0;
}

int SAR::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionSAR f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * SAR::dialog (Indicator &i)
{
  return new SARDialog(i);
}

void SAR::defaults (Indicator &i)
{
  Setting set;
  set.setData(SAR::Color, "yellow");
  set.setData(SAR::Label, _indicator);
  set.setData(SAR::Init, 0.02);
  set.setData(SAR::Max, 0.2);
  i.setSettings(set);
}

void SAR::plotNames (Indicator &i, QStringList &l)
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
  SAR *o = new SAR;
  return ((IndicatorPlugin *) o);
}


