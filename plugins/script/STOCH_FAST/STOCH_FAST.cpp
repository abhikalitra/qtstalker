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

#include "STOCH_FAST.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

STOCH_FAST::STOCH_FAST ()
{
  _plugin = "STOCH_FAST";
  _maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCH_FAST::STOCH_FAST: error on TA_Initialize");
}

int STOCH_FAST::command (Command *command)
{
  // PARMS:
  // NAME_FASTK
  // NAME_FASTD
  // PERIOD_FASTK
  // PERIOD_FASTD
  // MA_TYPE_FASTD

  BarData *data = g_barData;
  if (! data)
  {
    qDebug() << _plugin << "::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString kname = command->parm("NAME_FASTK");
  Curve *line = i->line(kname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_FASTK" << kname;
    return 1;
  }

  QString dname = command->parm("NAME_FASTD");
  line = i->line(dname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_FASTD" << dname;
    return 1;
  }

  bool ok;
  int kperiod = command->parm("PERIOD_FASTK").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FASTK" << command->parm("PERIOD_FASTK");
    return 1;
  }

  int dperiod = command->parm("PERIOD_FASTD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FASTD" << command->parm("PERIOD_FASTD");
    return 1;
  }

  int type = _maList.indexOf(command->parm("MA_TYPE_FASTD"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_FASTD" << command->parm("MA_TYPE_FASTD");
    return 1;
  }
  
  int size = data->count();

  if (size < kperiod || size < dperiod)
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

  TA_RetCode rc = TA_STOCHF(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            kperiod,
                            dperiod,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
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

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  STOCH_FAST *o = new STOCH_FAST;
  return ((ScriptPlugin *) o);
}
