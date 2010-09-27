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

#include "LINEARREG.h"
#include "FunctionLINEARREG.h"
#include "FunctionBARS.h"
#include "LINEARREGDialog.h"
#include "Curve.h"

#include <QtDebug>

LINEARREG::LINEARREG ()
{
  _indicator = "LINEARREG";
}

int LINEARREG::getIndicator (Indicator &ind, BarData &data)
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

  int period = settings.getInt(Period);

  FunctionLINEARREG f;
  QStringList methodList = f.list();
  
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  Curve *line = f.calculate(in, period, method);
  if (! line)
  {
    delete in;
    return 1;
  }

  switch ((FunctionLINEARREG::Method) method)
  {
    case FunctionLINEARREG::_LINEARREG:
    case FunctionLINEARREG::_INTERCEPT:
    {
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
      break;
    }
    default:
      break;
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

int LINEARREG::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionLINEARREG f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * LINEARREG::dialog (Indicator &i)
{
  return new LINEARREGDialog(i);
}

void LINEARREG::defaults (Indicator &i)
{
  Setting set;
  set.setData(Method, "LINEARREG");
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Input, "Close");
  set.setData(Period, 14);
  i.setSettings(set);
}

void LINEARREG::plotNames (Indicator &i, QStringList &l)
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
  LINEARREG *o = new LINEARREG;
  return ((IndicatorPlugin *) o);
}


