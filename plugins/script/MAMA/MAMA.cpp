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
#include "Globals.h"

#include <QtDebug>

MAMA::MAMA ()
{
  _plugin = "MAMA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MAMA::MAMA: error on TA_Initialize");
}

int MAMA::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_MAMA
  // NAME_FAMA
  // LIMIT_FAST
  // LIMIT_SLOW

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  QString mname = command->parm("NAME_MAMA");
  Curve *line = i->line(mname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_MAMA" << mname;
    return 1;
  }

  QString fname = command->parm("NAME_FAMA");
  line = i->line(fname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_FAMA" << fname;
    return 1;
  }

  bool ok;
  double flimit = command->parm("LIMIT_FAST").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LIMIT_FAST" << command->parm("LIMIT_FAST");
    return 1;
  }

  double slimit = command->parm("LIMIT_SLOW").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LIMIT_SLOW" << command->parm("LIMIT_FAST");
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
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
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

  command->setReturnCode("0");

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
