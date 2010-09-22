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

#include "AD.h"
#include "FunctionAD.h"
#include "ADDialog.h"
#include "Curve.h"

#include <QtDebug>

AD::AD ()
{
  _indicator = "AD";
}

int AD::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();

  FunctionAD f;
  QStringList methodList = f.list();
  
  QString s;
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  switch ((FunctionAD::Method) method)
  {
    case FunctionAD::_ADOSC:
    {
      int fp = settings.getInt(FastPeriod);
      int sp = settings.getInt(SlowPeriod);

      Curve *line = f.getADOSC(fp, sp);
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
      ind.setLine(s, line);
      break;
    }
    default:
    {
      Curve *line = f.getAD();
      if (! line)
	return 1;
      
      settings.getData(ADPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(ADColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(ADLabel, s);
      line->setLabel(s);

      line->setZ(0);
      ind.setLine(s, line);
      break;
    }
  }

  return 0;
}

int AD::getCUS (QStringList &set, Indicator &ind)
{
  FunctionAD f;
  return f.script(set, ind);
}

IndicatorPluginDialog * AD::dialog (Indicator &i)
{
  return new ADDialog(i);
}

void AD::defaults (Indicator &i)
{
  Setting set;
  set.setData(AD::Method, QString("AD"));
  set.setData(AD::ADColor, QString("red"));
  set.setData(AD::ADPlot, QString("Line"));
  set.setData(AD::ADLabel, QString("AD"));
  set.setData(AD::FastPeriod, 3);
  set.setData(AD::SlowPeriod, 10);
  set.setData(AD::OSCColor, QString("red"));
  set.setData(AD::OSCPlot, QString("Histogram Bar"));
  set.setData(AD::OSCLabel, QString("ADOSC"));
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  AD *o = new AD;
  return ((IndicatorPlugin *) o);
}

