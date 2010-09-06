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

#include "PP.h"
#include "FunctionPP.h"
#include "FunctionBARS.h"
#include "PPDialog.h"
#include "Curve.h"

#include <QtDebug>

PP::PP ()
{
  _indicator = "PP";
}

int PP::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  FunctionBARS b;
  Curve *bars = b.getBARS(data, up, down, neutral);
  if (bars)
  {
    bars->setZ(0);
    ind.setLine(0, bars);
  }

  FunctionPP f;
  QString s;
  // 1R line
  if (settings.getInt(R1Show))
  {
    Curve *line = f.calculate(data, 0);
    if (line)
    {
      settings.getData(R1Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(R1Label, s);
      line->setLabel(s);
      
      line->setZ(1);
      ind.setLine(1, line);
    }
  }

  // 2R line
  if (settings.getInt(R2Show))
  {
    Curve *line = f.calculate(data, 1);
    if (line)
    {
      settings.getData(R2Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(R2Label, s);
      line->setLabel(s);
      
      line->setZ(2);
      ind.setLine(2, line);
    }
  }

  // 3R line
  if (settings.getInt(R3Show))
  {
    Curve *line = f.calculate(data, 2);
    if (line)
    {
      settings.getData(R3Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(R3Label, s);
      line->setLabel(s);
      
      line->setZ(3);
      ind.setLine(3, line);
    }
  }

  // 1S line
  if (settings.getInt(S1Show))
  {
    Curve *line = f.calculate(data, 3);
    if (line)
    {
      settings.getData(S1Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(S1Label, s);
      line->setLabel(s);
      
      line->setZ(4);
      ind.setLine(4, line);
    }
  }

  // 2S line
  if (settings.getInt(S2Show))
  {
    Curve *line = f.calculate(data, 4);
    if (line)
    {
      settings.getData(S2Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(S2Label, s);
      line->setLabel(s);
      
      line->setZ(5);
      ind.setLine(5, line);
    }
  }

  // 3S line
  if (settings.getInt(S3Show))
  {
    Curve *line = f.calculate(data, 5);
    if (line)
    {
      settings.getData(S3Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(S3Label, s);
      line->setLabel(s);
      
      line->setZ(6);
      ind.setLine(6, line);
    }
  }

  return 0;
}

int PP::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionPP f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * PP::dialog (Indicator &i)
{
  return new PPDialog(i);
}

void PP::defaults (Indicator &i)
{
  Setting set;
  set.setData(R1Color, "red");
  set.setData(R2Color, "red");
  set.setData(R3Color, "red");
  set.setData(S1Color, "red");
  set.setData(S2Color, "red");
  set.setData(S3Color, "red");
  set.setData(R1Label, "1R");
  set.setData(R2Label, "2R");
  set.setData(R3Label, "3R");
  set.setData(S1Label, "1S");
  set.setData(S2Label, "2S");
  set.setData(S3Label, "3S");
  set.setData(R1Show, 1);
  set.setData(R2Show, 1);
  set.setData(R3Show, 1);
  set.setData(S1Show, 1);
  set.setData(S2Show, 1);
  set.setData(S3Show, 1);
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  PP *o = new PP;
  return ((IndicatorPlugin *) o);
}


