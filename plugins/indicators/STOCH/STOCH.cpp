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

#include "STOCH.h"
#include "FunctionMA.h"
#include "FunctionSTOCH.h"
#include "STOCHDialog.h"
#include "Curve.h"

#include <QtDebug>

STOCH::STOCH ()
{
  _indicator = "STOCH";
}

int STOCH::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // create first ref line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // create second ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  // create the fastk line
  int kperiod = settings.getInt(FastKPeriod);
  int dperiod = settings.getInt(FastDPeriod);

  FunctionMA mau;
  settings.getData(FastDMA, s);
  int maType = mau.typeFromString(s);

  FunctionSTOCH f;
  QList<Curve *> pl;
  if (f.calculate(data, kperiod, dperiod, maType, pl))
    return 1;

  line = pl.at(0);

  settings.getData(FastKPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(FastKColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(FastKLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(2, line);
  
  line = pl.at(1);

  settings.getData(FastDPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(FastDColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(FastDLabel, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(3, line);

  return 0;
}

int STOCH::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionSTOCH f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * STOCH::dialog (Indicator &i)
{
  return new STOCHDialog(i);
}

void STOCH::defaults (Indicator &i)
{
  Setting set;
  set.setData(FastKColor, "red");
  set.setData(FastDColor, "yellow");
  set.setData(Ref1Color, "white");
  set.setData(Ref2Color, "white");
  set.setData(FastKPlot, "Line");
  set.setData(FastDPlot, "Dash");
  set.setData(FastKLabel, "FASTK");
  set.setData(FastDLabel, "FASTD");
  set.setData(FastKPeriod, 5);
  set.setData(FastDPeriod, 3);
  set.setData(FastDMA, "SMA");
  set.setData(Ref1, 20);
  set.setData(Ref2, 80);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}


