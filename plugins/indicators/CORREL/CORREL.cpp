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
#include "Curve.h"
#include "ta_libc.h"
#include "ExchangeDataBase.h"

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
  settings.getData(_Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  BarData bd;
  settings.getData(_Input2, s);
  bd.setSymbol(s);
  settings.getData(_Exchange, s);
  bd.setExchange(s);
  bd.setBarLength(data.getBarLength());

  QStringList l;
  l << "Quotes" << "Date" << bd.getExchange() << bd.getSymbol();
  bd.barLengthText(bd.getBarLength(), s);
  l << s;
  Bar tbar = data.getBar(0);
  l << tbar.date().toString("yyyyMMddHHmmss");
  tbar = data.getBar(data.count() - 1);
  l << tbar.date().toString("yyyyMMddHHmmss");
  l << "-1";

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

  int period = settings.getInt(_Period);

  Curve *line = calculate(in, in2, period);
  if (! line)
  {
    delete in;
    delete in2;
    return 1;
  }

  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);

  line->setZ(3);
  ind.setLine(s, line);

  // 1 reference line
  Curve *hline = new Curve;
  hline->setType(Curve::Horizontal);

  settings.getData(_Ref3Color, s);
  c.setNamedColor(s);

  double ref = settings.getDouble(_Ref3);
  hline->setBar(0, new CurveBar(c, ref));

  hline->setZ(0);
  ind.setLine(0, hline);

  // 0 reference line
  hline = new Curve;
  hline->setType(Curve::Horizontal);

  settings.getData(_Ref2Color, s);
  c.setNamedColor(s);

  ref = settings.getDouble(_Ref2);
  hline->setBar(0, new CurveBar(c, ref));

  hline->setZ(1);
  ind.setLine(1, hline);

  // -1 reference line
  hline = new Curve;
  hline->setType(Curve::Horizontal);

  settings.getData(_Ref1Color, s);
  c.setNamedColor(s);

  ref = settings.getDouble(_Ref1);
  hline->setBar(0, new CurveBar(c, ref));

  hline->setZ(2);
  ind.setLine(2, hline);

  delete in;
  delete in2;

  return 0;
}

int CORREL::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CORREL,<NAME>,<INPUT_1>,<INPUT_2>,<PERIOD>
  //      0      1       2     3        4        5          6

  if (set.count() != 7)
  {
    qDebug() << "CORREL::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "CORREL::getCUS: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "CORREL::getCUS: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  Curve *in2 = ind.line(set[5]);
  if (! in2)
  {
    in2 = data.getInput(data.getInputType(set[5]));
    if (! in2)
    {
      qDebug() << "CORREL::getCUS: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in2);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "CORREL::getCUS: invalid period settings" << set[6];
    return 1;
  }

  Curve *line = calculate(in, in2, period);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * CORREL::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Color, d);
  dialog->addColor(tab, _Color, tr("Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 1);

  _settings.getData(_Input, d);
  dialog->addInput(tab, _Input, tr("Input"), d);

  ExchangeDataBase db;
  QStringList l;
  db.getExchanges(l);

  _settings.getData(_Exchange, d);
  dialog->addCombo(tab, _Exchange, tr("Index Exchange"), l, d);

  _settings.getData(_Input2, d);
  dialog->addInput(tab, _Input2, tr("Index Symbol"), d);

  // ref1 tab
  tab = dialog->addTab("Ref 1");

  _settings.getData(_Ref1Color, d);
  dialog->addColor(tab, _Ref1Color, tr("Color"), d);

  dialog->addDouble(tab, _Ref1, tr("Value"), _settings.getDouble(_Ref1), 100000, -100000);

  // ref2 tab
  tab = dialog->addTab("Ref 2");

  _settings.getData(_Ref2Color, d);
  dialog->addColor(tab, _Ref2Color, tr("Color"), d);

  dialog->addDouble(tab, _Ref2, tr("Value"), _settings.getDouble(_Ref2), 100000, -100000);

  // ref3 tab
  tab = dialog->addTab("Ref 3");

  _settings.getData(_Ref3Color, d);
  dialog->addColor(tab, _Ref3Color, tr("Color"), d);

  dialog->addDouble(tab, _Ref3, tr("Value"), _settings.getDouble(_Ref3), 100000, -100000);
  
  return dialog;
}

void CORREL::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_Input, "Close");
  set.setData(_Input2, "SP500");
  set.setData(_Exchange, "XNYS");
  set.setData(_Period, 30);
  set.setData(_Ref1Color, "white");
  set.setData(_Ref2Color, "white");
  set.setData(_Ref3Color, "white");
  set.setData(_Ref1, -1);
  set.setData(_Ref2, 0);
  set.setData(_Ref3, 1);
  i.setSettings(set);
}

void CORREL::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * CORREL::calculate (Curve *in, Curve *in2, int period)
{
  if (in->count() < period || in2->count() < period)
    return 0;

  QList<int> keys;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    input[loop] = (TA_Real) bar->data();
    input2[loop] = (TA_Real) bar2->data();
  }

  TA_RetCode rc = TA_CORREL(0,
                            size - 1,
                            &input[0],
                            &input2[0],
                            period,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "CORREL::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CORREL *o = new CORREL;
  return ((IndicatorPlugin *) o);
}


