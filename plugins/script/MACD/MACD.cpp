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
#include "Curve.h"
#include "ta_libc.h"
#include "FunctionMA.h"

#include <QtDebug>

MACD::MACD ()
{
}

int MACD::command (Command &command)
{
  // MACD,<INPUT>,<NAME_MACD>,<NAME_SIGNAL>,<NAME_HIST>,<FAST_PERIOD>,<FAST_MA_TYPE>,<SLOW_PERIOD>,<SLOW_MA_TYPE>,<SIGNAL_PERIOD>,<SIGNAL_MA_TYPE>
  //  0      1         2            3            4           5              6             7               8              9               10

  if (command.count() != 11)
  {
    qDebug() << "MACD::command: invalid settings count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "MACD::command: no indicator";
    return 1;
  }

  int pos = 1;
  Curve *in = i->line(command.parm(pos));
  if (! in)
  {
    qDebug() << "MACD::command: input missing" << command.parm(pos);
    return 1;
  }

  pos++;
  QString mname = command.parm(pos);
  Curve *line = i->line(mname);
  if (line)
  {
    qDebug() << "MACD::command: duplicate macd name" << mname;
    return 1;
  }

  pos++;
  QString sname = command.parm(pos);
  line = i->line(sname);
  if (line)
  {
    qDebug() << "MACD::command: duplicate signal name" << sname;
    return 1;
  }

  pos++;
  QString hname = command.parm(pos);
  line = i->line(hname);
  if (line)
  {
    qDebug() << "MACD::command: duplicate hist name" << hname;
    return 1;
  }

  pos++;
  bool ok;
  int fperiod = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::command: invalid fast period" << command.parm(pos);
    return 1;
  }

  pos++;
  FunctionMA fma;
  int ftype = fma.typeFromString(command.parm(pos));
  if (ftype == -1)
  {
    qDebug() << "MACD::command: invalid fast ma type" << command.parm(pos);
    return 1;
  }
  
  pos++;
  int speriod = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::command: invalid slow period" << command.parm(pos);
    return 1;
  }

  pos++;
  int stype = fma.typeFromString(command.parm(pos));
  if (stype == -1)
  {
    qDebug() << "MACD::command: invalid slow ma type" << command.parm(pos);
    return 1;
  }

  pos++;
  int sigperiod = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::command: invalid signal period" << command.parm(pos);
    return 1;
  }

  pos++;
  int sigtype = fma.typeFromString(command.parm(pos));
  if (sigtype == -1)
  {
    qDebug() << "MACD::command: invalid signal ma type" << command.parm(pos);
    return 1;
  }

  if (in->count() < fperiod || in->count() < speriod || in->count() < sigperiod)
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
                             fperiod,
                             (TA_MAType) ftype,
                             speriod,
                             (TA_MAType) stype,
                             sigperiod,
                             (TA_MAType) sigtype,
                             &outBeg,
                             &outNb,
                             &out[0],
                             &out2[0],
                             &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MACD::command: TA-Lib error" << rc;
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

  macd->setLabel(mname);
  signal->setLabel(sname);
  hist->setLabel(hname);

  i->setLine(mname, macd);
  i->setLine(sname, signal);
  i->setLine(hname, hist);

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  MACD *o = new MACD;
  return ((ScriptPlugin *) o);
}
