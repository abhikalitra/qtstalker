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

#include "AROON.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

AROON::AROON ()
{
}

int AROON::command (Command &command)
{
  // AROON,<UPPER NAME>,<LOWER NAME>,<PERIOD>
  //   0       1              2         3

  if (command.count() != 4)
  {
    qDebug() << "AROON::command: invalid settings count" << command.count();
    return 1;
  }

  BarData *data = command.barData();
  if (! data)
  {
    qDebug() << "AROON::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "AROON::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString uname = command.parm(pos);

  Curve *line = i->line(uname);
  if (line)
  {
    qDebug() << "AROON::command: duplicate upper name" << uname;
    return 1;
  }

  pos++;
  QString lname = command.parm(pos);

  line = i->line(lname);
  if (line)
  {
    qDebug() << "AROON::command: duplicate lower name" << lname;
    return 1;
  }

  pos++;
  bool ok;
  int period = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "AROON::command: invalid period" << command.parm(pos);
    return 1;
  }

  int size = data->count();
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->bar(loop);
    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
  }

  TA_RetCode rc = TA_AROON(0,
                         size - 1,
                         &high[0],
                         &low[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0],
                         &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "AROON::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *upper = new Curve;
  Curve *lower = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    upper->setBar(dataLoop, new CurveBar(out[outLoop]));
    lower->setBar(dataLoop, new CurveBar(out2[outLoop]));

    dataLoop--;
    outLoop--;
  }

  upper->setLabel(uname);
  lower->setLabel(lname);

  i->setLine(uname, upper);
  i->setLine(lname, lower);

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  AROON *o = new AROON;
  return ((ScriptPlugin *) o);
}
