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

#include "ROC.h"
#include "FunctionMA.h"
#include "ROCDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

ROC::ROC ()
{
  _indicator = "ROC";
  _methodList << "ROC" << "ROCP" << "ROCR" << "ROCR100";
}

int ROC::getIndicator (Indicator &ind, BarData &data)
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

  int period = settings.getInt(_Period);
  int smoothing = settings.getInt(_Smoothing);

  FunctionMA mau;
  settings.getData(_SmoothingType, s);
  int type = mau.typeFromString(s);

  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  Curve *line = calculate(in, period, method, smoothing, type);
  if (! line)
  {
    delete in;
    return 1;
  }

  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(0);
  ind.setLine(s, line);

  delete in;

  return 0;
}

int ROC::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,ROC,METHOD,<NAME>,<INPUT>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2    3      4       5       6             7                 8

  if (set.count() != 9)
  {
    qDebug() << "ROC::getCUS: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << "ROC::getCUS: invalid method" << set[3];
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "ROC::getCUS: duplicate name" << set[4];
    return 1;
  }

  Curve *in = ind.line(set[5]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[5]));
    if (! in)
    {
      qDebug() << "ROC::getCUS: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ROC::getCUS: invalid period" << set[6];
    return 1;
  }

  int smoothing = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "ROC::getCUS: invalid smoothing" << set[7];
    return 1;
  }

  FunctionMA mau;
  int type = mau.typeFromString(set[8]);
  if (type == -1)
  {
    qDebug() << "ROC::getCUS: invalid smoothing type" << set[8];
    return 1;
  }

  Curve *line = calculate(in, period, method, smoothing, type);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

IndicatorPluginDialog * ROC::dialog (Indicator &i)
{
  return new ROCDialog(i);
}

void ROC::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Histogram Bar");
  set.setData(_Label, _indicator);
  set.setData(_Period, 10);
  set.setData(_Smoothing, 1);
  set.setData(_SmoothingType, "SMA");
  set.setData(_Input, "Close");
  set.setData(_Method, "ROC");
  i.setSettings(set);
}

void ROC::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * ROC::calculate (Curve *in, int period, int method, int smoothing, int type)
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

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _ROCN:
      rc = TA_ROC(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ROCP:
      rc = TA_ROCP(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ROCR:
      rc = TA_ROCR(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ROCR100:
      rc = TA_ROCR100(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "ROC::calculate: TA-Lib error" << rc;
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

QStringList & ROC::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ROC *o = new ROC;
  return ((IndicatorPlugin *) o);
}


