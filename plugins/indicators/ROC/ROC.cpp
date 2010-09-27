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

#include "ROC.h"
#include "FunctionMA.h"
#include "ROCDialog.h"
#include "FunctionROC.h"
#include "Curve.h"

#include <QtDebug>

ROC::ROC ()
{
  _indicator = "ROC";
}

int ROC::getIndicator (Indicator &ind, BarData &data)
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
  int smoothing = settings.getInt(Smoothing);

  FunctionMA mau;
  settings.getData(SmoothingType, s);
  int type = mau.typeFromString(s);

  FunctionROC f;
  QStringList methodList = f.list();
  
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  Curve *line = f.calculate(in, period, method, smoothing, type);
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

int ROC::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionROC f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * ROC::dialog (Indicator &i)
{
  return new ROCDialog(i);
}

void ROC::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Histogram Bar");
  set.setData(Label, _indicator);
  set.setData(Period, 10);
  set.setData(Smoothing, 1);
  set.setData(SmoothingType, "SMA");
  set.setData(Input, "Close");
  set.setData(Method, "ROC");
  i.setSettings(set);
}

void ROC::plotNames (Indicator &i, QStringList &l)
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
  ROC *o = new ROC;
  return ((IndicatorPlugin *) o);
}


