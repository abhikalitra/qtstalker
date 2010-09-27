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

#include "ADX.h"
#include "Curve.h"
#include "FunctionADX.h"
#include "ADXDialog.h"

#include <QtDebug>

ADX::ADX ()
{
  _indicator = "ADX";
}

int ADX::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  int period = settings.getInt(Period);

  FunctionADX f;

  if (settings.getInt(MDICheck))
  {
    Curve *line = f.calculate(period, (int) FunctionADX::_MDI, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(MDIPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(MDIColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(MDILabel, s);
    line->setLabel(s);

    line->setZ(0);
    ind.setLine(0, line);
  }

  if (settings.getInt(PDICheck))
  {
    Curve *line = f.calculate(period, (int) FunctionADX::_PDI, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(PDIPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(PDIColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(PDILabel, s);
    line->setLabel(s);
    
    line->setZ(1);
    ind.setLine(1, line);
  }

  if (settings.getInt(ADXCheck))
  {
    Curve *line = f.calculate(period, (int) FunctionADX::_ADX, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(ADXPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(ADXColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(ADXLabel, s);
    line->setLabel(s);
    
    line->setZ(2);
    ind.setLine(2, line);
  }

  if (settings.getInt(ADXRCheck))
  {
    Curve *line = f.calculate(period, (int) FunctionADX::_ADXR, data);
    if (! line)
      return 1;

    QString s;
    settings.getData(ADXRPlot, s);
    line->setType((Curve::Type) line->typeFromString(s));

    settings.getData(ADXRColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(ADXRLabel, s);
    line->setLabel(s);
    
    line->setZ(3);
    ind.setLine(3, line);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionADX f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * ADX::dialog (Indicator &i)
{
  return new ADXDialog(i);
}

void ADX::defaults (Indicator &i)
{
  Setting set;
  set.setData(ADXColor, "blue");
  set.setData(ADXRColor, "yellow");
  set.setData(PDIColor, "green");
  set.setData(MDIColor, "red");
  set.setData(ADXPlot, "Line");
  set.setData(ADXRPlot, "Line");
  set.setData(PDIPlot, "Line");
  set.setData(MDIPlot, "Line");
  set.setData(ADXLabel, "ADX");
  set.setData(ADXRLabel, "ADXR");
  set.setData(PDILabel, "+DI");
  set.setData(MDILabel, "-DI");
  set.setData(ADXCheck, 1);
  set.setData(ADXRCheck, 1);
  set.setData(PDICheck, 1);
  set.setData(MDICheck, 1);
  set.setData(Period, 14);
  i.setSettings(set);
}

void ADX::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ADX *o = new ADX;
  return ((IndicatorPlugin *) o);
}


