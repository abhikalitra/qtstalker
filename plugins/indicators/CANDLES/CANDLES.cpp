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

#include "CANDLES.h"
#include "FunctionMA.h"
#include "FunctionCANDLES.h"
#include "CANDLESDialog.h"
#include "Curve.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  _indicator = "CANDLES";
}

int CANDLES::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();

  FunctionCANDLES f;
  Curve *line = f.candles();
  if (! line)
    return 1;

  QString s;
  settings.getData(Color, s);
  QColor c(s);
  line->setAllColor(c);

  settings.getData(Label, s);
  line->setLabel(s);

  QStringList methodList = f.list();
  settings.getData(Method, s);
  int method = methodList.indexOf(s);
  if (method != FunctionCANDLES::_NONE)
  {
    double pen = settings.getDouble(Penetration);

    Curve *line2 = f.getMethod(method, pen);
    if (line2)
    {
      settings.getData(MethodColor, s);
      QColor color(s);
      
      int loop = 0;
      for (; loop < line2->count(); loop++)
      {
        CurveBar *bar2 = line2->bar(loop);
        if (bar2->data() > 0)
        {
          CurveBar *bar = line->bar(loop);
          bar->setColor(color);
        }
      }
      
      delete line2;
    }
  }

  line->setZ(0);

  ind.setLine(0, line);

  int period = settings.getInt(MAPeriod);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(MAType, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MAPlot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MAColor, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(MALabel, s);
      ma->setLabel(s);
      
      ind.setLine(1, ma);

      ma->setZ(1);
    }
  }

  period = settings.getInt(MA2Period);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(MA2Type, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MA2Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MA2Color, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(MA2Label, s);
      ma->setLabel(s);
      
      ind.setLine(2, ma);
      
      ma->setZ(2);
    }
  }

  period = settings.getInt(MA3Period);
  if (period > 1)
  {
    FunctionMA mau;
    settings.getData(MA3Type, s);
    int type = mau.typeFromString(s);

    Curve *ma = mau.calculate(line, period, type);
    if (ma)
    {
      settings.getData(MA3Plot, s);
      ma->setType((Curve::Type) ma->typeFromString(s));

      settings.getData(MA3Color, s);
      c.setNamedColor(s);
      ma->setColor(c);

      settings.getData(MA3Label, s);
      ma->setLabel(s);
      
      ind.setLine(3, ma);
      
      ma->setZ(3);
    }
  }

  return 0;
}

int CANDLES::getCUS (QStringList &set, Indicator &ind)
{
  FunctionCANDLES f;
  return f.script(set, ind);
}

IndicatorPluginDialog * CANDLES::dialog (Indicator &i)
{
  return new CANDLESDialog(i);
}

void CANDLES::defaults (Indicator &i)
{
  Setting set;
  set.setData(CANDLES::Penetration, 0.3);
  set.setData(CANDLES::Method, "NONE");
  set.setData(CANDLES::MethodColor, "cyan");
  set.setData(CANDLES::Color, "green");
  set.setData(CANDLES::Label, _indicator);
  set.setData(CANDLES::MAColor, "red");
  set.setData(CANDLES::MA2Color, "yellow");
  set.setData(CANDLES::MA3Color, "blue");
  set.setData(CANDLES::MAPlot, "Line");
  set.setData(CANDLES::MA2Plot, "Line");
  set.setData(CANDLES::MA3Plot, "Line");
  set.setData(CANDLES::MALabel, "MA1");
  set.setData(CANDLES::MA2Label, "MA2");
  set.setData(CANDLES::MA3Label, "MA3");
  set.setData(CANDLES::MAPeriod, 20);
  set.setData(CANDLES::MA2Period, 50);
  set.setData(CANDLES::MA3Period, 200);
  set.setData(CANDLES::MAType, "SMA");
  set.setData(CANDLES::MA2Type, "SMA");
  set.setData(CANDLES::MA3Type, "SMA");
  i.setSettings(set);
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CANDLES *o = new CANDLES;
  return ((IndicatorPlugin *) o);
}


