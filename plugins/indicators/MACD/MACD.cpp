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

#include "MACD.h"
#include "FunctionMA.h"
#include "MACDDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

MACD::MACD ()
{
  _indicator = "MACD";
}

int MACD::getIndicator (Indicator &ind, BarData &data)
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

  int fast = settings.getInt(_FastPeriod);
  int slow = settings.getInt(_SlowPeriod);
  int signal = settings.getInt(_SignalPeriod);

  FunctionMA mau;
  settings.getData(_FastMA, s);
  int fastma = mau.typeFromString(s);

  settings.getData(_SlowMA, s);
  int slowma = mau.typeFromString(s);

  settings.getData(_SignalMA, s);
  int signalma = mau.typeFromString(s);

  QList<Curve *> pl;
  if (calculate(in, fast, fastma, slow, slowma, signal, signalma, pl))
  {
    delete in;
    return 1;
  }

  // plot hist 
  Curve *line = pl.at(2);

  settings.getData(_HistPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_HistColor, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_HistLabel, s);
  line->setLabel(s);
  
  line->setZ(0);
  ind.setLine(0, line);

  // plot macd
  line = pl.at(0);

  settings.getData(_MACDPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_MACDColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(_MACDLabel, s);
  line->setLabel(s);

  line->setZ(1);
  ind.setLine(1, line);

  // plot signal
  line = pl.at(1);

  settings.getData(_SignalPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_SignalColor, s);
  c.setNamedColor(s);
  line->setColor(c);

  settings.getData(_SignalLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(2, line);

  delete in;

  return 0;
}

int MACD::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,MACD,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,<FAST_MA_TYPE>,<SLOW_PERIOD>,<SLOW_MA_TYPE>,<SIGNAL_PERIOD>,<SIGNAL_MA_TYPE>
  //     0       1     2      3         4           5             6           7               8            9             10                11              12

  if (set.count() != 13)
  {
    qDebug() << "MACD::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "MACD::getCUS: duplicate name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "MACD::getCUS: duplicate name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << "MACD::getCUS: duplicate name" << set[6];
    return 1;
  }

  Curve *in = ind.line(set[3]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[3]));
    if (! in)
    {
      qDebug() << "MACD::getCUS: input not found" << set[3];
      return 1;
    }

    ind.setLine(set[3], in);
  }

  bool ok;
  int fast = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::getCUS: invalid fast period" << set[7];
    return 1;
  }

  FunctionMA mau;
  int fastma = mau.typeFromString(set[8]);
  if (fastma == -1)
  {
    qDebug() << "MACD::getCUS: invalid fast ma" << set[8];
    return 1;
  }

  int slow = set[9].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::getCUS: invalid slow period" << set[9];
    return 1;
  }

  int slowma = mau.typeFromString(set[10]);
  if (slowma == -1)
  {
    qDebug() << "MACD::getCUS: invalid slow ma" << set[10];
    return 1;
  }

  int signal = set[11].toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::getCUS: invalid signal period" << set[11];
    return 1;
  }

  int signalma = mau.typeFromString(set[12]);
  if (signalma == -1)
  {
    qDebug() << "MACD::getCUS: invalid fast ma" << set[12];
    return 1;
  }

  QList<Curve *> pl;
  if (calculate(in, fast, fastma, slow, slowma, signal, signalma, pl))
    return 1;

  Curve *line = pl.at(0);
  line->setLabel(set[4]);
  ind.setLine(set[4], line);

  line = pl.at(1);
  line->setLabel(set[5]);
  ind.setLine(set[5], line);

  line = pl.at(2);
  line->setLabel(set[6]);
  ind.setLine(set[6], line);

  return 0;
}

IndicatorPluginDialog * MACD::dialog (Indicator &i)
{
  return new MACDDialog(i);
}

void MACD::defaults (Indicator &i)
{
  Setting set;
  set.setData(_MACDColor, "red");
  set.setData(_SignalColor, "yellow");
  set.setData(_HistColor, "blue");
  set.setData(_MACDPlot, "Line");
  set.setData(_SignalPlot, "Line");
  set.setData(_HistPlot, "Histogram");
  set.setData(_MACDLabel, "MACD");
  set.setData(_SignalLabel, "SIG");
  set.setData(_HistLabel, "HIST");
  set.setData(_FastPeriod, 12);
  set.setData(_SlowPeriod, 26);
  set.setData(_SignalPeriod, 9);
  set.setData(_FastMA, "SMA");
  set.setData(_SlowMA, "SMA");
  set.setData(_SignalMA, "EMA");
  set.setData(_Input, "Close");
  i.setSettings(set);
}

void MACD::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_MACDLabel, s);
  l.append(s);
  
  settings.getData(_SignalLabel, s);
  l.append(s);
  
  settings.getData(_HistLabel, s);
  l.append(s);
}

int MACD::calculate (Curve *in, int fastPeriod, int fastMA, int slowPeriod, int slowMA, int signalPeriod,
                     int signalMA, QList<Curve *> &pl)
{
  if (in->count() < fastPeriod || in->count() < slowPeriod || in->count() < signalPeriod)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MACDEXT(0,
                             size - 1,
                             &input[0],
                             fastPeriod,
                             (TA_MAType) fastMA,
                             slowPeriod,
                             (TA_MAType) slowMA,
                             signalPeriod,
                             (TA_MAType) signalMA,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MACD::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *macd = new Curve;
  Curve *signal = new Curve;
  Curve *hist = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    macd->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    signal->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));
    hist->setBar(keys.at(keyLoop), new CurveBar(out3[outLoop]));

    keyLoop--;
    outLoop--;
  }

  pl.append(macd);
  pl.append(signal);
  pl.append(hist);

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MACD *o = new MACD;
  return ((IndicatorPlugin *) o);
}


