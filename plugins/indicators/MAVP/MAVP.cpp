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

#include "MAVP.h"
#include "FunctionMAVP.h"
#include "FunctionMA.h"
#include "FunctionBARS.h"
#include "MAVPDialog.h"
#include "Curve.h"

#include <QtDebug>

MAVP::MAVP ()
{
  _indicator = "MAVP";
}

int MAVP::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  settings.getData(Input2, s);
  Curve *in2 = data.getInput(data.getInputType(s));
  if (! in2)
  {
    delete in;
    qDebug() << _indicator << "::getIndicator: input 2 not found" << s;
    return 1;
  }

  int min = settings.getInt(Min);
  int max = settings.getInt(Max);

  FunctionMA mau;
  settings.getData(MAType, s);
  int ma = mau.typeFromString(s);

  FunctionMAVP f;
  Curve *line = f.calculate(in, in2, min, max, ma);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  settings.getData(Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(Label, s);
  line->setLabel(s);

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

  line->setZ(1);
  ind.setLine(1, line);

  delete in;
  delete in2;

  return 0;
}

int MAVP::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionMAVP f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * MAVP::dialog (Indicator &i)
{
  return new MAVPDialog(i);
}

void MAVP::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Input, "Close");
  set.setData(Input2, "Close");
  set.setData(Min, 2);
  set.setData(Max, 30);
  set.setData(MAType, "SMA");
  i.setSettings(set);
}

void MAVP::plotNames (Indicator &i, QStringList &l)
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
  MAVP *o = new MAVP;
  return ((IndicatorPlugin *) o);
}


