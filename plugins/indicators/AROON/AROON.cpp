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

#include "AROON.h"
#include "Curve.h"
#include "FunctionAROON.h"
#include "AROONDialog.h"

#include <QtDebug>

AROON::AROON ()
{
  _indicator = "AROON";
}

int AROON::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  FunctionAROON f;
  QStringList methodList = f.list();
  
  QString s;
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  int period = settings.getInt(Period);

  switch ((FunctionAROON::Method) method)
  {
    case FunctionAROON::_AROONOSC:
    {
      Curve *line = f.getAROONOSC(period, data);
      if (! line)
        return 1;

      settings.getData(OSCPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(OSCColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(OSCLabel, s);
      line->setLabel(s);

      line->setZ(0);
      ind.setLine(0, line);
      break;
    }
    default:
    {
      QList<Curve *> pl;
      if (f.getAROON(period, pl, data))
        return 1;

      Curve *line = pl.at(0);

      settings.getData(UpPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(UpColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(UpLabel, s);
      line->setLabel(s);
      
      line->setZ(1);
      ind.setLine(1, line);

      line = pl.at(1);

      settings.getData(DownPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(DownColor, s);
      c.setNamedColor(s);
      line->setColor(c);

      settings.getData(DownLabel, s);
      line->setLabel(s);
      
      line->setZ(2);
      ind.setLine(2, line);
      break;
    }
  }

  return 0;
}

int AROON::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionAROON f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * AROON::dialog (Indicator &i)
{
  return new AROONDialog(i);
}

void AROON::defaults (Indicator &i)
{
  Setting set;
  set.setData(Method, "AROON");
  set.setData(DownColor, "red");
  set.setData(UpColor, "green");
  set.setData(DownPlot, "Line");
  set.setData(UpPlot, "Line");
  set.setData(DownLabel, "AROOND");
  set.setData(UpLabel, "AROONU");
  set.setData(OSCColor, "red");
  set.setData(OSCPlot, "Histogram Bar");
  set.setData(OSCLabel, "AROONOSC");
  set.setData(Period, 14);
  i.setSettings(set);
}

void AROON::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  AROON *o = new AROON;
  return ((IndicatorPlugin *) o);
}


