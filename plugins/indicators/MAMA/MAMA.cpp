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

#include "MAMA.h"
#include "FunctionMAMA.h"
#include "FunctionBARS.h"
#include "MAMADialog.h"
#include "Curve.h"

#include <QtDebug>

MAMA::MAMA ()
{
  _indicator = "MAMA";
}

int MAMA::getIndicator (Indicator &ind, BarData &data)
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

  double fast = settings.getDouble(FastLimit);
  double slow = settings.getDouble(SlowLimit);

  FunctionMAMA f;
  QList<Curve *> l;
  if (f.calculate(in, fast, slow, l))
  {
    delete in;
    return 1;
  }

  Curve *mama = l.at(0);
  Curve *fama = l.at(1);

  int osc = settings.getInt(OSC);
  if (osc)
  {
    Curve *line = new Curve;
    line->setType(Curve::HistogramBar);

    settings.getData(OSCLabel, s);
    line->setLabel(s);

    QList<int> keys;
    mama->keys(keys);

    int loop = 0;
    for (; loop < keys.count(); loop++)
    {
      CurveBar *mbar = mama->bar(keys.at(loop));
      if (! mbar)
        continue;

      CurveBar *fbar = fama->bar(keys.at(loop));
      if (! fbar)
        continue;

      line->setBar(keys.at(loop), new CurveBar(mbar->data() - fbar->data()));
    }

    settings.getData(OSCColor, s);
    QColor c(s);
    line->setColor(c);

    line->setZ(0);
    ind.setLine(0, line);
  }
  else
  {
    QColor up("green");
    QColor down("red");
    QColor neutral("blue");
    FunctionBARS b;
    Curve *bars = b.getBARS(data, up, down, neutral);
    if (bars)
    {
      bars->setZ(1);
      ind.setLine(1, bars);
    }

    settings.getData(MAMAPlot, s);
    mama->setType((Curve::Type) mama->typeFromString(s));

    settings.getData(MAMAColor, s);
    QColor c(s);
    mama->setColor(c);

    settings.getData(MAMALabel, s);
    mama->setLabel(s);
    
    mama->setZ(2);
    ind.setLine(2, mama);

    // fama line
    settings.getData(FAMAPlot, s);
    fama->setType((Curve::Type) fama->typeFromString(s));

    settings.getData(FAMAColor, s);
    c.setNamedColor(s);
    fama->setColor(c);

    settings.getData(FAMALabel, s);
    fama->setLabel(s);
    
    fama->setZ(3);
    ind.setLine(3, fama);
  }

  delete in;

  return 0;
}

int MAMA::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionMAMA f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * MAMA::dialog (Indicator &i)
{
  return new MAMADialog(i);
}

void MAMA::defaults (Indicator &i)
{
  Setting set;
  set.setData(OSC, 1);
  set.setData(OSCColor, "red");
  set.setData(MAMAColor, "red");
  set.setData(FAMAColor, "yellow");
  set.setData(MAMAPlot, "Line");
  set.setData(FAMAPlot, "Line");
  set.setData(OSCPlot, "Histogram Bar");
  set.setData(MAMALabel, "MAMA");
  set.setData(FAMALabel, "FAMA");
  set.setData(OSCLabel, "MAMAOSC");
  set.setData(FastLimit, 0.5);
  set.setData(SlowLimit, 0.05);
  set.setData(Input, "Close");
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MAMA *o = new MAMA;
  return ((IndicatorPlugin *) o);
}


