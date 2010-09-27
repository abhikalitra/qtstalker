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

#include "BETA.h"
#include "QuoteServerRequest.h"
#include "FunctionBETA.h"
#include "BETADialog.h"
#include "Curve.h"

#include <QtDebug>

BETA::BETA ()
{
  _indicator = "BETA";
}

int BETA::getIndicator (Indicator &ind, BarData &data)
{
  if (! data.count())
    return 1;
  
  Setting settings = ind.settings();

  QString s;
  settings.getData(Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  BarData bd;
  settings.getData(Index, s);
  bd.setSymbol(s);
  settings.getData(Exchange, s);
  bd.setExchange(s);
  bd.setBarLength(data.getBarLength());

  QStringList l;
  l << "GetQuotes" << bd.getExchange() << bd.getSymbol();
  bd.barLengthText(bd.getBarLength(), s);
  l << s;
  Bar bar = data.getBar(0);
  l << bar.date().toString("yyyyMMddHHmmss");
  bar = data.getBar(data.count() - 1);
  l << bar.date().toString("yyyyMMddHHmmss");

  QString command = l.join(",") + "\n";

  QuoteServerRequest qsr;
  if (qsr.run(command))
    return 1;

  bd.setBars(qsr.data());

  Curve *in2 = bd.getInput(BarData::Close);
  if (! in2)
  {
    qDebug() << _indicator << "::calculate: index not found";
    delete in;
    return 1;
  }

  int period = settings.getInt(Period);

  FunctionBETA f;
  Curve *line = f.calculate(in, in2, period);
  if (! line)
  {
    delete in;
    delete in2;
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
  delete in2;

  return 0;
}

int BETA::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionBETA f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * BETA::dialog (Indicator &i)
{
  return new BETADialog(i);
}

void BETA::defaults (Indicator &i)
{
  Setting set;
  set.setData(Index, "SP500");
  set.setData(Exchange, "XNYS");
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Input, "Close");
  set.setData(Period, 5);
  i.setSettings(set);
}

void BETA::plotNames (Indicator &i, QStringList &l)
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
  BETA *o = new BETA;
  return ((IndicatorPlugin *) o);
}


