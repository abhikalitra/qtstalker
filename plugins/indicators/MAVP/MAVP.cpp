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

#include "MAVP.h"
#include "FunctionMA.h"
#include "FunctionBARS.h"
#include "MAVPDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

MAVP::MAVP ()
{
  _indicator = "MAVP";
}

int MAVP::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(_Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  settings.getData(_Input2, s);
  Curve *in2 = data.getInput(data.getInputType(s));
  if (! in2)
  {
    delete in;
    qDebug() << _indicator << "::getIndicator: input 2 not found" << s;
    return 1;
  }

  int min = settings.getInt(_Min);
  int max = settings.getInt(_Max);

  FunctionMA mau;
  settings.getData(_MAType, s);
  int ma = mau.typeFromString(s);

  Curve *line = calculate(in, in2, min, max, ma);
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

  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  FunctionBARS b;
  Curve *bars = b.getBARS(up, down, neutral, data);
  if (bars)
  {
    bars->setZ(0);
    ind.setLine(0, bars);
  }

  settings.getData(_Label, s);
  line->setLabel(s);

  line->setZ(1);
  ind.setLine(s, line);

  delete in;
  delete in2;

  return 0;
}

int MAVP::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,MAVP,<NAME>,<INPUT_1>,<INPUT_2>,<MIN_PERIOD>,<MAX_PERIOD>,<MA_TYPE>
  //     0       1     2     3        4        5           6            7          8

  if (set.count() != 9)
  {
    qDebug() << "MAVP::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "MAVP::getCUS: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "MAVP::getCUS: input not found" << set[4];
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
      qDebug() << "MAVP::getCUS: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in2);
  }

  bool ok;
  int min = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MAVP::getCUS: invalid min period settings" << set[6];
    return 1;
  }

  int max = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MAVP::getCUS: invalid max period settings" << set[7];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << "MAVP::getCUS: invalid ma settings" << set[8];
    return 1;
  }

  Curve *line = calculate(in, in2, min, max, ma);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * MAVP::dialog (Indicator &i)
{
  return new MAVPDialog(i);
}

void MAVP::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_Input, "Close");
  set.setData(_Input2, "Close");
  set.setData(_Min, 2);
  set.setData(_Max, 30);
  set.setData(_MAType, "SMA");
  i.setSettings(set);
}

void MAVP::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * MAVP::calculate (Curve *in, Curve *in2, int min, int max, int ma)
{
  if (in->count() < min || in->count() < max)
    return 0;

  int flag = 0;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    flag = 1;
  }

  QList<int> keys;
  in->keys(keys);
  QList<int> keys2;
  in2->keys(keys2);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = keys.count() - 1;
  int loop2 = keys2.count() - 1;
  while (loop > -1 && loop2 > -1)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    CurveBar *bar2 = in2->bar(keys2.at(loop2));
    input[loop] = (TA_Real) bar->data();
    input2[loop2] = (TA_Real) bar2->data();

    loop--;
    loop2--;
  }

  TA_RetCode rc = TA_MAVP(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          min,
                          max,
                          (TA_MAType) ma,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAVP::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  if (! flag)
  {
    int keyLoop = keys.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }
  else
  {
    int keyLoop = keys2.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys2.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MAVP *o = new MAVP;
  return ((IndicatorPlugin *) o);
}


