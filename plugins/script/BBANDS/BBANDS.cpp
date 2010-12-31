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

#include "BBANDS.h"
#include "Curve.h"
#include "ta_libc.h"
#include "FunctionMA.h"
#include "Globals.h"

#include <QtDebug>

BBANDS::BBANDS ()
{
}

int BBANDS::command (Command *command)
{
  // BBANDS,<INPUT>,<NAME UPPER>,<NAME MIDDLE>,<NAME LOWER>,<PERIOD>,<MA_TYPE>,<UP DEV>,<LOW DEV>
  //   0       1         2             3            4           5        6        7         8


  if (command->count() != 9)
  {
    qDebug() << "BBANDS::command: invalid settings count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "BBANDS::command: no indicator";
    return 1;
  }

  int pos = 1;
  Curve *input = i->line(command->parm(pos));
  if (! input)
  {
    qDebug() << "BBANDS::command: input missing" << command->parm(pos);
    return 1;
  }

  pos++;
  QString uname = command->parm(pos);
  Curve *line = i->line(uname);
  if (line)
  {
    qDebug() << "BBANDS::command: duplicate upper name" << uname;
    return 1;
  }

  pos++;
  QString mname = command->parm(pos);
  line = i->line(mname);
  if (line)
  {
    qDebug() << "BBANDS::command: duplicate middle name" << mname;
    return 1;
  }

  pos++;
  QString lname = command->parm(pos);
  line = i->line(lname);
  if (line)
  {
    qDebug() << "BBANDS::command: duplicate lower name" << lname;
    return 1;
  }

  pos++;
  bool ok;
  int period = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::command: invalid period" << command->parm(pos);
    return 1;
  }

  if (input->count() < period)
    return 1;

  pos++;
  FunctionMA fma;
  int type = fma.typeFromString(command->parm(pos));
  if (type == -1)
  {
    qDebug() << "BBANDS::command: invalid ma type" << command->parm(pos);
    return 1;
  }
  
  pos++;
  double udev = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::command: invalid upper deviation" << command->parm(pos);
    return 1;
  }

  pos++;
  double ldev = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "BBANDS::command: invalid lower deviation" << command->parm(pos);
    return 1;
  }

  int size = input->count();
  TA_Real in[size];
  TA_Real uout[size];
  TA_Real mout[size];
  TA_Real lout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  input->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = input->bar(keys.at(loop));
    in[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_BBANDS(0,
                            keys.count() - 1,
                            &in[0],
                            period,
                            udev,
                            ldev,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &uout[0],
                            &mout[0],
                            &lout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "BBANDS::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *upper = new Curve;
  Curve *middle = new Curve;
  Curve *lower = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    upper->setBar(keys.at(keyLoop), new CurveBar(uout[outLoop]));
    middle->setBar(keys.at(keyLoop), new CurveBar(mout[outLoop]));
    lower->setBar(keys.at(keyLoop), new CurveBar(lout[outLoop]));

    keyLoop--;
    outLoop--;
  }

  upper->setLabel(uname);
  middle->setLabel(mname);
  lower->setLabel(lname);

  i->setLine(uname, upper);
  i->setLine(mname, middle);
  i->setLine(lname, lower);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  BBANDS *o = new BBANDS;
  return ((ScriptPlugin *) o);
}
