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
#include "FunctionBARS.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "ta_libc.h"
#include "IndicatorSettings.h"

#include <QtDebug>
#include <cmath>

MA::MA ()
{
  _indicator = "MA";
}

int MA::getIndicator (Indicator &ind, BarData &data)
{
  IndicatorSettings settings = ind.settings();

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

  IndicatorSetting set = settings.setting(_Input);
  Curve *in = data.getInput(data.getInputType(set.value()));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << set.value();
    return 1;
  }

  set = settings.setting(_Period);
  int period = set.value().toInt();

  FunctionMA fma;
  set = settings.setting(_Method);
  int method = fma.typeFromString(set.value());

  Curve *line = fma.calculate(in, period, method);
  if (! line)
  {
    delete in;
    return 1;
  }

  set = settings.setting(_Plot);
  line->setType((Curve::Type) line->typeFromString(set.value()));

  set = settings.setting(_Color);
  QColor c(set.value());
  line->setColor(c);

  set = settings.setting(_Label);
  line->setLabel(set.value());
  
  line->setZ(1);
  ind.setLine(set.value(), line);

  delete in;

  return 0;
}

int MA::getCUS (QStringList &l, Indicator &ind, BarData &data)
{
  IndicatorSettings settings = ind.settings();
  if (settings.setCUS(l))
    return 1;

  IndicatorSetting set = settings.setting(_Label);
  QString name = set.value();
  Curve *tl = ind.line(name);
  if (tl)
  {
    qDebug() << "MA::getCUS: duplicate name" << set.value();
    return 1;
  }

  set = settings.setting(_Input);
  Curve *in = ind.line(set.value());
  if (! in)
  {
    in = data.getInput(data.getInputType(set.value()));
    if (! in)
    {
      qDebug() << "MA::getCUS: input not found" << set.value();
      return 1;
    }

    ind.setLine(set.value(), in);
  }

  set = settings.setting(_Period);
  int period = set.value().toInt();

  set = settings.setting(_Method);
  FunctionMA fma;
  int method = fma.typeFromString(set.value());

  Curve *line = fma.calculate(in, period, method);
  if (! line)
    return 1;

  line->setLabel(name);

  ind.setLine(name, line);

  return 0;
}

IndicatorPluginDialog * MA::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);
  dialog->setHelpFile("MA.html");
  return dialog;
}

void MA::defaults (Indicator &i)
{
  IndicatorSettings set;

  QStringList l;
  l << tr("General");
  set.setTabs(l);

  // INDICATOR,PLUGIN,MA,<METHOD>,<NAME>,<INPUT>,<PERIOD>
  //     0       1    2     3       4       5       6

  l.clear();
  l << "INDICATOR" << "PLUGIN" << "MA" << QString::number(_Method);
  l << QString::number(_Label) << QString::number(_Input) << QString::number(_Period);
  set.setCUSFormat(l);

  int page = 0;
  set.setColor(_Color, page, tr("Color"), QString("red"));
  set.setPlot(_Plot, page, tr("Plot"), QString("Line"));
  set.setText(_Label, page, tr("Label"), _indicator);
  set.setMA(_Method, page, tr("Method"), QString("SMA"));
  set.setInput(_Input, page, tr("Input"), QString("Close"));
  set.setInteger(_Period, page, tr("Period"), QString("14"));

  i.setSettings(set);
}

void MA::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  IndicatorSettings settings = i.settings();

  IndicatorSetting set = settings.setting(_Label);
  l.append(set.value());
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MA *o = new MA;
  return ((IndicatorPlugin *) o);
}
