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

#include "STOCHRSI.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

STOCHRSI::STOCHRSI ()
{
  _indicator = "STOCHRSI";
}

int STOCHRSI::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // create first ref line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // create second ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  settings.getData(_Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  int period = settings.getInt(_Period);

  line = calculate(in, period);
  if (! line)
  {
    delete in;
    return 1;
  }
  
  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(s, line);

  delete in;

  return 0;
}

int STOCHRSI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,STOCHRSI,<NAME>,<INPUT>,<PERIOD>
  //    0        1       2       3       4       5

  if (set.count() != 6)
  {
    qDebug() << "STOCHRSI::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "STOCHRSI::getCUS: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "STOCHRSI::getCUS: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHRSI::getCUS: invalid period" << set[5];
    return 1;
  }

  Curve *line = calculate(in, period);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * STOCHRSI::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("STOCHRSI.html");

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Input, d);
  dialog->addInput(tab, _Input, tr("Input"), d);

  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 2);

  _settings.getData(_Color, d);
  dialog->addColor(tab, _Color, tr("Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

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

  return dialog;
}

void STOCHRSI::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Ref1Color, "white");
  set.setData(_Ref2Color, "white");
  set.setData(_Plot, "Line");
  set.setData(_Label, "STOCHRSI");
  set.setData(_Ref1, 0.2);
  set.setData(_Ref2, 0.8);
  set.setData(_Input, "Close");
  set.setData(_Period, 14);
  i.setSettings(set);
}

void STOCHRSI::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * STOCHRSI::calculate (Curve *in, int period)
{
  if (in->count() < period)
    return 0;

  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_STOCHRSI(0,
                              size - 1,
                              &input[0],
                              period,
                              period,
                              1,
                              (TA_MAType) 0,
                              &outBeg,
                              &outNb,
                              &out[0],
                              &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCHRSI::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCHRSI *o = new STOCHRSI;
  return ((IndicatorPlugin *) o);
}


