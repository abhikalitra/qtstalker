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

#include "STOCHS.h"
#include "FunctionMA.h"
#include "FunctionSTOCHS.h"
#include "STOCHSDialog.h"
#include "Curve.h"

#include <QtDebug>

STOCHS::STOCHS ()
{
  _indicator = "STOCHS";
}

int STOCHS::getIndicator (Indicator &ind)
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

  int fkperiod = settings.getInt(FastKPeriod);
  int skperiod = settings.getInt(SlowKPeriod);

  FunctionMA mau;
  settings.getData(SlowKMA, s);
  int kmaType = mau.typeFromString(s);

  int dperiod = settings.getInt(SlowDPeriod);

  settings.getData(SlowDMA, s);
  int dmaType = mau.typeFromString(s);

  FunctionSTOCHS f;
  QList<Curve *> pl;
  if (f.calculate(fkperiod, skperiod, dperiod, kmaType, dmaType, pl))
    return 1;

  line = pl.at(0);

  settings.getData(SlowKPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(SlowKColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(SlowKLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(2, line);

  line = pl.at(1);

  settings.getData(SlowDPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(SlowDColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(SlowDLabel, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(3, line);

  return 0;
}

int STOCHS::getCUS (QStringList &set, Indicator &ind)
{
  FunctionSTOCHS f;
  return f.script(set, ind);
}

IndicatorPluginDialog * STOCHS::dialog (Indicator &i)
{
  return new STOCHSDialog(i);
}

void STOCHS::defaults (Indicator &i)
{
  Setting set;
  set.setData(SlowKColor, "red");
  set.setData(SlowDColor, "yellow");
  set.setData(Ref1Color, "white");
  set.setData(Ref2Color, "white");
  set.setData(SlowKPlot, "Line");
  set.setData(SlowDPlot, "Dash");
  set.setData(SlowKLabel, "SLOWK");
  set.setData(SlowDLabel, "SLOWD");
  set.setData(FastKPeriod, 5);
  set.setData(SlowKPeriod, 3);
  set.setData(SlowDPeriod, 3);
  set.setData(SlowKMA, "SMA");
  set.setData(SlowDMA, "SMA");
  set.setData(Ref1, 20);
  set.setData(Ref2, 80);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCHS *o = new STOCHS;
  return ((IndicatorPlugin *) o);
}


