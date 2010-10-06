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

#include "VIDYA.h"
#include "VIDYADialog.h"
#include "FunctionBARS.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>
#include <cmath>
#include <QVector>

#define PI 3.14159265

VIDYA::VIDYA ()
{
  _indicator = "VIDYA";
}

int VIDYA::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

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
  
  QString s;
  settings.getData(_Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  int period = settings.getInt(_Period);
  int volPeriod = settings.getInt(_VPeriod);

  Curve *line = calculate(in, period, volPeriod);
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
  
  line->setZ(1);
  ind.setLine(s, line);

  delete in;

  return 0;
}

int VIDYA::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,VIDYA,<NAME>,<INPUT>,<PERIOD>,<VOLUME_PERIOD>
  //     0       1      2     3       4       5            6

  if (set.count() != 7)
  {
    qDebug() << "VIDYA::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "VIDYA::getCUS: duplicate name" << set[3];
    return 1;
  }

  Curve *inSignal = ind.line(set[4]);
  if (! inSignal)
  {
    inSignal = data.getInput(data.getInputType(set[4]));
    if (! inSignal)
    {
      qDebug() << "VIDYA::getCUS: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], inSignal);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "VIDYA::getCUS: invalid fast period settings" << set[5];
    return 1;
  }

  int volPeriod = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "VIDYA::getCUS: invalid fast period settings" << set[6];
    return 1;
  }

  Curve *line = calculate(inSignal, period, volPeriod);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * VIDYA::dialog (Indicator &i)
{
  return new VIDYADialog(i);
}

void VIDYA::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_Period, 14);
  set.setData(_Input, "Close");
  set.setData(_VPeriod, 10);
  i.setSettings(set);
}

void VIDYA::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * VIDYA::calculate (Curve *inSignal, int period, int volPeriod)
{
  if (inSignal->count() < period || inSignal->count() < volPeriod)
    return 0;

  Curve *cmo = getCMO(inSignal, volPeriod);
  if (! cmo)
    return 0;

  Curve *out = new Curve;

  int size = inSignal->count();
  QVector<double> *inSeries = new QVector<double>(size);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(size);
  offset->fill(0.0);
  QVector<double> *absCmo = new QVector<double>(size);
  absCmo->fill(0.0);
  QVector<double> *vidya = new QVector<double>(size);
  vidya->fill(0.0);

  double c = 2 / (double) period + 1;

  QList<int> keys;
  inSignal->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = inSignal->bar(keys.at(loop));
    (*inSeries)[loop] = bar->data();
  }

  keys.clear();
  cmo->keys(keys);

  int index = inSeries->size() -1;
  loop = keys.count() - 1;
  for (; loop > -1; loop--)
  {
    CurveBar *bar = cmo->bar(keys.at(loop));
    (*absCmo)[index] = fabs(bar->data() / 100);
    index--;
  }

  loop = volPeriod + period;
  for (; loop < (int) inSeries->size(); loop++)         // period safty
  {
    (*vidya)[loop] = (inSeries->at(loop) * c * absCmo->at(loop)) + ((1 - absCmo->at(loop) * c) * vidya->at(loop - 1));
    //!  (Price*Const*AbsCMO) + ((1-AbsCMO*Const)*FunctionVIDYA[1]),Price);

    out->setBar(loop, new CurveBar(vidya->at(loop)));
  }

  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;

  return out;
}

Curve * VIDYA::getCMO (Curve *in, int period)
{
  if (in->count() < period)
    return 0;

  TA_Real input[in->count()];
  TA_Real out[in->count()];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_CMO(0,
                         keys.count() - 1,
                         &input[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "VIDYA::getCMO: TA-Lib error" << rc;
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
  VIDYA *o = new VIDYA;
  return ((IndicatorPlugin *) o);
}

