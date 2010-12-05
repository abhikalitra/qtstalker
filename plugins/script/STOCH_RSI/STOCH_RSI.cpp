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

#include "STOCH_RSI.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

STOCH_RSI::STOCH_RSI ()
{
}

int STOCH_RSI::command (Command &command)
{
  // STOCH_RSI,<NAME>,<INPUT>,<PERIOD>
  //  0     1      2       3

  if (command.count() != 4)
  {
    qDebug() << "STOCH_RSI::command: invalid settings count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "STOCH_RSI::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command.parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "STOCH_RSI::command: duplicate name" << name;
    return 1;
  }

  pos++;
  Curve *in = i->line(command.parm(pos));
  if (! in)
  {
    qDebug() << "STOCH_RSI::command: input missing" << command.parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int period = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH_RSI::command: invalid period" << command.parm(pos);
    return 1;
  }

  if (in->count() < period)
    return 1;

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
    qDebug() << "STOCH_RSI::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  line->setLabel(name);

  i->setLine(name, line);

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  STOCH_RSI *o = new STOCH_RSI;
  return ((ScriptPlugin *) o);
}
