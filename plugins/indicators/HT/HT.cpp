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

#include "HT.h"
#include "FunctionHT.h"
#include "FunctionBARS.h"
#include "HTDialog.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

HT::HT ()
{
  _indicator = "HT";
}

int HT::getIndicator (Indicator &ind)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(Input, s);
  Curve *in = g_barData.getInput(g_barData.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  FunctionHT f;
  QStringList methodList = f.list();
  
  settings.getData(Method, s);
  int method = methodList.indexOf(s);

  switch ((FunctionHT::Method) method)
  {
    case FunctionHT::_PHASOR:
    {
      QList<Curve *> pl;
      if (f.getPHASE(in, pl))
      {
        delete in;
        return 1;
      }

      Curve *line = pl.at(0);

      settings.getData(PhasePlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(PhaseColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(PhaseLabel, s);
      line->setLabel(s);

      line->setZ(0);
      ind.setLine(0, line);

      line = pl.at(1);

      settings.getData(QuadPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(QuadColor, s);
      c.setNamedColor(s);
      line->setColor(c);

      settings.getData(QuadLabel, s);
      line->setLabel(s);
      
      line->setZ(1);
      ind.setLine(1, line);
      break;
    }
    case FunctionHT::_SINE:
    {
      QList<Curve *> pl;
      if (f.getSINE(in, pl))
      {
        delete in;
        return 1;
      }
      
      Curve *line = pl.at(0);

      settings.getData(SinePlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(SineColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(SineLabel, s);
      line->setLabel(s);
      
      line->setZ(2);
      ind.setLine(2, line);

      line = pl.at(1);

      settings.getData(LeadPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(LeadColor, s);
      c.setNamedColor(s);
      line->setColor(c);

      settings.getData(LeadLabel, s);
      line->setLabel(s);
      
      line->setZ(3);
      ind.setLine(3, line);
      break;
    }
    default:
    {
      QColor up("green");
      QColor down("red");
      QColor neutral("blue");
      FunctionBARS b;
      Curve *bars = b.getBARS(up, down, neutral);
      if (bars)
      {
        bars->setZ(4);
        ind.setLine(4, bars);
      }

      Curve *line = f.getHT(in, method);
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
      
      line->setZ(5);
      ind.setLine(5, line);
      break;
    }
  }

  delete in;
  return 0;
}

int HT::getCUS (QStringList &set, Indicator &ind)
{
  FunctionHT f;
  return f.script(set, ind);
}

IndicatorPluginDialog * HT::dialog (Indicator &i)
{
  return new HTDialog(i);
}

void HT::defaults (Indicator &i)
{
  Setting set;
  set.setData(Method, "TRENDLINE");
  set.setData(Color, "red");
  set.setData(PhaseColor, "red");
  set.setData(QuadColor, "yellow");
  set.setData(SineColor, "red");
  set.setData(LeadColor, "yellow");
  set.setData(Plot, "Line");
  set.setData(PhasePlot, "Line");
  set.setData(QuadPlot, "Line");
  set.setData(SinePlot, "Line");
  set.setData(LeadPlot, "Line");
  set.setData(Label, _indicator);
  set.setData(PhaseLabel, "PHASE");
  set.setData(QuadLabel, "QUAD");
  set.setData(SineLabel, "SINE");
  set.setData(LeadLabel, "LEAD");
  set.setData(Input, "Close");
  i.setSettings(set);
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  HT *o = new HT;
  return ((IndicatorPlugin *) o);
}


