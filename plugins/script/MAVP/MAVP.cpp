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

#include "MAVP.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "MAVPDialog.h"
#include "InputType.h"
#include "MAType.h"

#include <QtDebug>

MAVP::MAVP ()
{
  _plugin = "MAVP";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MAVP::MAVP: error on TA_Initialize");
}

int MAVP::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  int min = settings->getInt("PERIOD_MIN");
  int max = settings->getInt("PERIOD_MAX");

  MAType mat;
  int type = mat.fromString(settings->data("MA_TYPE"));
  
  Curve *in = i->line(settings->data("INPUT"));
  if (! in)
  {
    InputType it;
    in = it.input(bd, settings->data("INPUT"));
    if (! in)
    {
      qDebug() << _plugin << "::calculate: no input" << settings->data("INPUT");
      return 1;
    }

    in->setZ(-1);
    i->setLine(settings->data("INPUT"), in);
  }

  Curve *in2 = i->line(settings->data("INPUT2"));
  if (! in2)
  {
    InputType it;
    in2 = it.input(bd, settings->data("INPUT2"));
    if (! in2)
    {
      qDebug() << _plugin << "::calculate: no input" << settings->data("INPUT2");
      return 1;
    }

    in2->setZ(-1);
    i->setLine(settings->data("INPUT2"), in2);
  }

  int flag = 0;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    flag = 1;
  }

  QList<int> keys;
  in->keys(keys);
  QList<int> keys2;
  in2->keys(keys2);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = keys.count() - 1;
  int loop2 = keys2.count() - 1;
  while (loop > -1 && loop2 > -1)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    CurveBar *bar2 = in2->bar(keys2.at(loop2));
    input[loop] = (TA_Real) bar->data();
    input2[loop2] = (TA_Real) bar2->data();

    loop--;
    loop2--;
  }

  TA_RetCode rc = TA_MAVP(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          min,
                          max,
                          (TA_MAType) type,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *line = new Curve;

  if (! flag)
  {
    int keyLoop = keys.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }
  else
  {
    int keyLoop = keys2.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys2.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }

  line->setAllColor(QColor(settings->data("COLOR")));
  line->setLabel(settings->data("OUTPUT"));
  line->setType(settings->data("STYLE"));
  line->setZ(settings->getInt("Z"));
  i->setLine(settings->data("OUTPUT"), line);

  return 0;
}

int MAVP::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // INPUT2
  // PERIOD_MIN
  // PERIOD_MAX
  // MA_TYPE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  Curve *in2 = i->line(command->parm("INPUT2"));
  if (! in2)
  {
    qDebug() << _plugin << "::command: INPUT2 missing" << command->parm("INPUT2");
    return 1;
  }

  bool ok;
  int min = command->parm("PERIOD_MIN").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_MIN" << command->parm("PERIOD_MIN");
    return 1;
  }

  int max = command->parm("PERIOD_MAX").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_MAX" << command->parm("PERIOD_MAX");
    return 1;
  }

  MAType mat;
  int type = mat.fromString(command->parm("MA_TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE" << command->parm("MA_TYPE");
    return 1;
  }

  if (in->count() < min || in->count() < max)
    return 1;

  int flag = 0;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    flag = 1;
  }

  QList<int> keys;
  in->keys(keys);
  QList<int> keys2;
  in2->keys(keys2);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = keys.count() - 1;
  int loop2 = keys2.count() - 1;
  while (loop > -1 && loop2 > -1)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    CurveBar *bar2 = in2->bar(keys2.at(loop2));
    input[loop] = (TA_Real) bar->data();
    input2[loop2] = (TA_Real) bar2->data();

    loop--;
    loop2--;
  }

  TA_RetCode rc = TA_MAVP(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          min,
                          max,
                          (TA_MAType) type,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  if (! flag)
  {
    int keyLoop = keys.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }
  else
  {
    int keyLoop = keys2.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys2.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * MAVP::dialog (QWidget *p, Setting *set)
{
  return new MAVPDialog(p, set);
}

void MAVP::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData("INPUT", QString("Close"));
  set->setData("INPUT2", QString("Close"));
  set->setData("COLOR", QString("red"));
  set->setData("STYLE", QString("Line"));
  set->setData("PERIOD_MIN", 2);
  set->setData("PERIOD_MAX", 30);
  set->setData("MA_TYPE", QString("EMA"));
  set->setData("Z", 0);
  set->setData("OUTPUT", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  MAVP *o = new MAVP;
  return ((Plugin *) o);
}
