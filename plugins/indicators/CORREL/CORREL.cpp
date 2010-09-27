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

#include "CORREL.h"
#include "QuoteServerRequest.h"
#include "FunctionCORREL.h"
#include "CORRELDialog.h"
#include "Curve.h"

#include <QtDebug>

CORREL::CORREL ()
{
  _indicator = "CORREL";
}

int CORREL::getIndicator (Indicator &ind, BarData &data)
{
  if (! data.count())
    return 1;

  Setting settings = ind.settings();

  QString s;
  settings.getData(Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  BarData bd;
  settings.getData(Input2, s);
  bd.setSymbol(s);
  settings.getData(Exchange, s);
  bd.setExchange(s);
  bd.setBarLength(data.getBarLength());

  QStringList l;
  l << "GetQuotes" << bd.getExchange() << bd.getSymbol();
  bd.barLengthText(bd.getBarLength(), s);
  l << s;
  Bar tbar = data.getBar(0);
  l << tbar.date().toString("yyyyMMddHHmmss");
  tbar = data.getBar(data.count() - 1);
  l << tbar.date().toString("yyyyMMddHHmmss");

  QString command = l.join(",") + "\n";

  QuoteServerRequest qsr;
  if (qsr.run(command))
    return 1;

  bd.setBars(qsr.data());

  Curve *in2 = bd.getInput(BarData::Close);
  if (! in2)
  {
    qDebug() << _indicator << "::getIndicator: input 2 not found";
    delete in;
    return 1;
  }

  int period = settings.getInt(Period);

  FunctionCORREL f;
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

  line->setZ(3);
  ind.setLine(3, line);

  // 1 reference line
  Curve *hline = new Curve;
  hline->setType(Curve::Horizontal);

  settings.getData(Ref3Color, s);
  c.setNamedColor(s);

  double ref = settings.getDouble(Ref3);
  hline->setBar(0, new CurveBar(c, ref));

  hline->setZ(0);
  ind.setLine(0, hline);

  // 0 reference line
  hline = new Curve;
  hline->setType(Curve::Horizontal);

  settings.getData(Ref2Color, s);
  c.setNamedColor(s);

  ref = settings.getDouble(Ref2);
  hline->setBar(0, new CurveBar(c, ref));

  hline->setZ(1);
  ind.setLine(1, hline);

  // -1 reference line
  hline = new Curve;
  hline->setType(Curve::Horizontal);

  settings.getData(Ref1Color, s);
  c.setNamedColor(s);

  ref = settings.getDouble(Ref1);
  hline->setBar(0, new CurveBar(c, ref));

  hline->setZ(2);
  ind.setLine(2, hline);

  delete in;
  delete in2;

  return 0;
}

int CORREL::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  FunctionCORREL f;
  return f.script(set, ind, data);
}

IndicatorPluginDialog * CORREL::dialog (Indicator &i)
{
  return new CORRELDialog(i);
}

void CORREL::defaults (Indicator &i)
{
  Setting set;
  set.setData(Color, "red");
  set.setData(Plot, "Line");
  set.setData(Label, _indicator);
  set.setData(Input, "Close");
  set.setData(Input2, "SP500");
  set.setData(Exchange, "XNYS");
  set.setData(Period, 30);
  set.setData(Ref1Color, "white");
  set.setData(Ref2Color, "white");
  set.setData(Ref3Color, "white");
  set.setData(Ref1, -1);
  set.setData(Ref2, 0);
  set.setData(Ref3, 1);
  i.setSettings(set);
}

void CORREL::plotNames (Indicator &i, QStringList &l)
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
  CORREL *o = new CORREL;
  return ((IndicatorPlugin *) o);
}


