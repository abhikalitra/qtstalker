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

#include "MACD.h"
#include "FunctionMA.h"
#include "MACDDialog.h"
#include "FunctionMACD.h"
#include "Curve.h"

#include <QtDebug>

MACD::MACD ()
{
  _indicator = "MACD";
}

int MACD::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int fast = settings.getInt(FastPeriod);
  int slow = settings.getInt(SlowPeriod);
  int signal = settings.getInt(SignalPeriod);

  FunctionMA mau;
  settings.getData(FastMA, s);
  int fastma = mau.typeFromString(s);

  settings.getData(SlowMA, s);
  int slowma = mau.typeFromString(s);

  settings.getData(SignalMA, s);
  int signalma = mau.typeFromString(s);

  FunctionMACD f;
  QList<Curve *> pl;
  if (f.calculate(in, fast, fastma, slow, slowma, signal, signalma, pl))
  {
    delete in;
    return 1;
  }

  // plot hist 
  Curve *line = pl.at(2);

  settings.getData(HistPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(HistColor, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(HistLabel, s);
  line->setLabel(s);
  
  line->setZ(0);
  ind.setLine(0, line);

  // plot macd
  line = pl.at(0);

  settings.getData(MACDPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(MACDColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(MACDLabel, s);
  line->setLabel(s);

  line->setZ(1);
  ind.setLine(1, line);

  // plot signal
  line = pl.at(1);

  settings.getData(SignalPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(SignalColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(SignalLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(2, line);

  delete in;

  return 0;
}

int MACD::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionMACD f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * MACD::dialog (Indicator &i)
{
  return new MACDDialog(i);
}

void MACD::defaults (Indicator &i)
{
  Setting set;
  set.setData(MACDColor, "red");
  set.setData(SignalColor, "yellow");
  set.setData(HistColor, "blue");
  set.setData(MACDPlot, "Line");
  set.setData(SignalPlot, "Line");
  set.setData(HistPlot, "Histogram");
  set.setData(MACDLabel, "MACD");
  set.setData(SignalLabel, "SIG");
  set.setData(HistLabel, "HIST");
  set.setData(FastPeriod, 12);
  set.setData(SlowPeriod, 26);
  set.setData(SignalPeriod, 9);
  set.setData(FastMA, "SMA");
  set.setData(SlowMA, "SMA");
  set.setData(SignalMA, "EMA");
  set.setData(Input, "Close");
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MACD *o = new MACD;
  return ((IndicatorPlugin *) o);
}


