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
#include "Globals.h"

#include <QtDebug>

STOCH_SLOW::STOCH_SLOW ()
{
  _plugin = "STOCH_SLOW";
  _maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("STOCH_SLOW::STOCH_SLOW: error on TA_Initialize");
}

int STOCH_SLOW::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME_SLOWK
  // NAME_SLOWD
  // PERIOD_FASTK
  // PERIOD_SLOWK
  // MA_TYPE_SLOWK
  // PERIOD_SLOWD
  // MA_TYPE_SLOWD

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *ihigh = i->line(command->parm("INPUT_HIGH"));
  if (! ihigh)
  {
    qDebug() << _plugin << "::command: invalid INPUT_HIGH" << command->parm("INPUT_HIGH");
    return 1;
  }

  Curve *ilow = i->line(command->parm("INPUT_LOW"));
  if (! ilow)
  {
    qDebug() << _plugin << "::command: invalid INPUT_LOW" << command->parm("INPUT_LOW");
    return 1;
  }

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  QString kname = command->parm("NAME_SLOWK");
  Curve *line = i->line(kname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_SLOWK" << kname;
    return 1;
  }

  QString dname = command->parm("NAME_SLOWD");
  line = i->line(dname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_SLOWD" << dname;
    return 1;
  }

  bool ok;
  int fkperiod = command->parm("PERIOD_FASTK").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_FASTK" << command->parm("PERIOD_FASTK");
    return 1;
  }

  int skperiod = command->parm("PERIOD_SLOWK").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SLOWK" << command->parm("PERIOD_SLOWK");
    return 1;
  }

  int kma = _maList.indexOf(command->parm("MA_TYPE_SLOWK"));
  if (kma == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_SLOWK" << command->parm("MA_TYPE_SLOWK");
    return 1;
  }

  int sdperiod = command->parm("PERIOD_SLOWD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SLOWD" << command->parm("PERIOD_SLOWD");
    return 1;
  }

  int dma = _maList.indexOf(command->parm("MA_TYPE_SLOWD"));
  if (dma == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE_SLOWD" << command->parm("MA_TYPE_SLOWD");
    return 1;
  }

  int size = iclose->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];

  int ipos = 0;
  int opos = 0;
  int end = 0;
  iclose->keyRange(ipos, end);
  for (; ipos <= end; ipos++, opos++)
  {
    CurveBar *hbar = ihigh->bar(ipos);
    if (! hbar)
      continue;
    
    CurveBar *lbar = ilow->bar(ipos);
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(ipos);
    if (! cbar)
      continue;

    high[opos] = (TA_Real) hbar->data();
    low[opos] = (TA_Real) lbar->data();
    close[opos] = (TA_Real) cbar->data();
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
  STOCH_SLOW *o = new STOCH_SLOW;
  return ((ScriptPlugin *) o);
}
