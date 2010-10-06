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

#include "RSI.h"
#include "FunctionMA.h"
#include "RSIDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

RSI::RSI ()
{
  _indicator = "RSI";
}

int RSI::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // ref1 line
  Curve *line = new Curve;
  line->setType((Curve::Horizontal));

  QString s;
  settings.getData(_Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // ref2 line
  line = new Curve;
  line->setType((Curve::Horizontal));

  settings.getData(_Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref2)));

  line->setZ(0);
  ind.setLine(1, line);

  settings.getData(_Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(_Period);
  int smoothing = settings.getInt(_Smoothing);

  FunctionMA mau;
  settings.getData(_SmoothingType, s);
  int type = mau.typeFromString(s);

  line = calculate(in, period, smoothing, type);
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

int RSI::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,RSI,<NAME>,<INPUT>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0        1    2     3      4       5              6                 7

  if (set.count() != 10)
  {
    qDebug() << "RSI::getCUS: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "RSI::getCUS: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "RSI::getCUS: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "RSI::getCUS: invalid period parm" << set[5];
    return 1;
  }

  int smoothing = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "RSI::getCUS: invalid smoothing" << set[6];
    return 1;
  }

  FunctionMA mau;
  int type = mau.typeFromString(set[7]);
  if (type == -1)
  {
    qDebug() << "RSI::getCUS: invalid smoothing type" << set[7];
    return 1;
  }

  Curve *line = calculate(in, period, smoothing, type);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * RSI::dialog (Indicator &i)
{
  return new RSIDialog(i);
}

void RSI::defaults (Indicator &i)
{
  Setting set;
  set.setData(RSI::_Color, "red");
  set.setData(RSI::_Plot, "Line");
  set.setData(RSI::_Label, _indicator);
  set.setData(RSI::_Period, 14);
  set.setData(RSI::_Smoothing, 1);
  set.setData(RSI::_SmoothingType, "SMA");
  set.setData(RSI::_Input, "Close");
  set.setData(RSI::_Ref1Color, "white");
  set.setData(RSI::_Ref2Color, "white");
  set.setData(RSI::_Ref1, 30);
  set.setData(RSI::_Ref2, 70);
  i.setSettings(set);
}

void RSI::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * RSI::calculate (Curve *in, int period, int smoothing, int type)
{
  if (in->count() < period || in->count() < smoothing)
    return 0;

  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_RSI(0,
                         size - 1,
                         &input[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "RSI::calculate: TA-Lib error" << rc;
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

  if (smoothing > 1)
  {
    FunctionMA mau;
    Curve *ma = mau.calculate(line, smoothing, type);
    delete line;
    line = ma;
  }

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  RSI *o = new RSI;
  return ((IndicatorPlugin *) o);
}


