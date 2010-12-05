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

#include "MAMA.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

MAMA::MAMA ()
{
}

int MAMA::command (Command &command)
{
  // MAMA,<INPUT>,<NAME_MAMA>,<NAME_FAMA>,<FAST_LIMIT>,<SLOW_LIMIT>
  //   0     1         2           3           4            5

  if (command.count() != 6)
  {
    qDebug() << "MAMA::command: invalid settings count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "MAMA::command: no indicator";
    return 1;
  }

  int pos = 1;
  Curve *in = i->line(command.parm(pos));
  if (! in)
  {
    qDebug() << "MAMA::command: input missing" << command.parm(pos);
    return 1;
  }

  pos++;  
  QString mname = command.parm(pos);
  Curve *line = i->line(mname);
  if (line)
  {
    qDebug() << "MAMA::command: duplicate mama name" << mname;
    return 1;
  }

  pos++;
  QString fname = command.parm(pos);
  line = i->line(fname);
  if (line)
  {
    qDebug() << "MAMA::command: duplicate fama name" << fname;
    return 1;
  }

  pos++;
  bool ok;
  int flimit = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::command: invalid fast limit" << command.parm(pos);
    return 1;
  }

  pos++;
  int slimit = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MACD::command: invalid slow limit" << command.parm(pos);
    return 1;
  }

  if (in->count() < flimit || in->count() < slimit)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &input[0],
                          flimit,
                          slimit,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAMA::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *mama = new Curve;
  Curve *fama = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    mama->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    fama->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  mama->setLabel(mname);
  fama->setLabel(fname);

  i->setLine(mname, mama);
  i->setLine(fname, fama);

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  MAMA *o = new MAMA;
  return ((ScriptPlugin *) o);
}
