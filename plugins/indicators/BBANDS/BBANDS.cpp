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

#include "BBANDS.h"
#include "FunctionMA.h"
#include "FunctionBARS.h"
#include "Curve.h"
#include "FunctionBBANDS.h"
#include "BBANDSDialog.h"

#include <QtDebug>
#include <QColor>

BBANDS::BBANDS ()
{
  _indicator = "BBANDS";
}

int BBANDS::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(BarsUpColor, s);
  QColor up(s);
  
  settings.getData(BarsDownColor, s);
  QColor down(s);
  
  settings.getData(BarsNeutralColor, s);
  QColor neutral(s);
  
  FunctionBARS b;
  Curve *bars = b.getBARS(up, down, neutral, data);
  if (! bars)
    return 1;

  bars->setZ(0);
  ind.setLine(0, bars);

  settings.getData(Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(Period);
  double udev = settings.getDouble(UpDeviation);
  double ldev = settings.getDouble(DownDeviation);

  FunctionMA mau;
  settings.getData(MAType, s);
  int maType = mau.typeFromString(s);

  QList<Curve *> pl;
  FunctionBBANDS f;
  if (f.calculate(in, period, udev, ldev, maType, pl))
  {
    delete in;
    return 1;
  }

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

  settings.getData(MidPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(MidColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(MidLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(2, line);

  line = pl.at(2);

  settings.getData(DownPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(DownColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(DownLabel, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(3, line);

  delete in;

  return 0;
}

int BBANDS::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionBBANDS f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * BBANDS::dialog (Indicator &i)
{
  return new BBANDSDialog(i);
}

void BBANDS::defaults (Indicator &i)
{
  Setting set;
  set.setData(UpColor, "red");
  set.setData(MidColor, "red");
  set.setData(DownColor, "red");
  set.setData(UpPlot, "Line");
  set.setData(MidPlot, "Line");
  set.setData(DownPlot, "Line");
  set.setData(UpLabel, "BBU");
  set.setData(MidLabel, "BBM");
  set.setData(DownLabel, "BBL");
  set.setData(UpDeviation, 2);
  set.setData(DownDeviation, 2);
  set.setData(Input, "Close");
  set.setData(Period, 20);
  set.setData(MAType, "SMA");
  set.setData(BarsUpColor, "green");
  set.setData(BarsDownColor, "red");
  set.setData(BarsNeutralColor, "blue");
  set.setData(BarsLabel, "BARS");
  i.setSettings(set);
}

void BBANDS::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(UpLabel, s);
  l.append(s);

  settings.getData(MidLabel, s);
  l.append(s);

  settings.getData(DownLabel, s);
  l.append(s);
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BBANDS *o = new BBANDS;
  return ((IndicatorPlugin *) o);
}


