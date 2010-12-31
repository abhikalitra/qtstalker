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

#include "STOCH_SLOW.h"
#include "Curve.h"
#include "ta_libc.h"
#include "FunctionMA.h"
#include "Globals.h"

#include <QtDebug>

STOCH_SLOW::STOCH_SLOW ()
{
}

int STOCH_SLOW::command (Command *command)
{
  // STOCH_SLOW,<NAME SLOWK>,<NAME SLOWD>,<FASTK PERIOD>,<SLOWK PERIOD>,<SLOWK MA TYPE>,<SLOWD PERIOD>,<SLOWD MA TYPE>
  //     0           1            2             3              4               5              6               7

  if (command->count() != 8)
  {
    qDebug() << "STOCH_SLOW::command: invalid settings count" << command->count();
    return 1;
  }

  BarData *data = g_barData;
  if (! data)
  {
    qDebug() << "STOCH_SLOW::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "STOCH_SLOW::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString kname = command->parm(pos);
  Curve *line = i->line(kname);
  if (line)
  {
    qDebug() << "STOCH_SLOW::command: duplicate slow k name" << kname;
    return 1;
  }

  pos++;
  QString dname = command->parm(pos);
  line = i->line(dname);
  if (line)
  {
    qDebug() << "STOCH_SLOW::command: duplicate slow d name" << dname;
    return 1;
  }

  pos++;
  bool ok;
  int fkperiod = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH_SLOW::command: invalid fast k period" << command->parm(pos);
    return 1;
  }

  pos++;
  int skperiod = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH_SLOW::command: invalid slow k period" << command->parm(pos);
    return 1;
  }

  pos++;
  FunctionMA fma;
  int kma = fma.typeFromString(command->parm(pos));
  if (kma == -1)
  {
    qDebug() << "STOCH_SLOW::command: invalid slow k ma type" << command->parm(pos);
    return 1;
  }

  pos++;
  int sdperiod = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH_SLOW::command: invalid slow d period" << command->parm(pos);
    return 1;
  }

  pos++;
  int dma = fma.typeFromString(command->parm(pos));
  if (dma == -1)
  {
    qDebug() << "STOCH_SLOW::command: invalid slow d ma type" << command->parm(pos);
    return 1;
  }

  int size = data->count();

  if (size < fkperiod || size < skperiod)
    return 1;

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->bar(loop);
    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
    close[loop] = (TA_Real) bar->close();
  }

  TA_RetCode rc = TA_STOCH(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           fkperiod,
                           skperiod,
                           (TA_MAType) kma,
                           sdperiod,
                           (TA_MAType) dma,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCH_SLOW::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *kline = new Curve;
  Curve *dline = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    kline->setBar(dataLoop, new CurveBar(out[outLoop]));
    dline->setBar(dataLoop, new CurveBar(out2[outLoop]));

    dataLoop--;
    outLoop--;
  }

  kline->setLabel(kname);
  dline->setLabel(dname);

  i->setLine(kname, kline);
  i->setLine(dname, dline);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  STOCH_SLOW *o = new STOCH_SLOW;
  return ((ScriptPlugin *) o);
}
