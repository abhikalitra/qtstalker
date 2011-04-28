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

#include "ULTOSC.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"
#include "InputType.h"

#include <QtDebug>

ULTOSC::ULTOSC ()
{
  _plugin = "ULTOSC";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("ULTOSC::ULTOSC: error on TA_Initialize");
}

int ULTOSC::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME
  // PERIOD_SHORT
  // PERIOD_MED
  // PERIOD_LONG

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

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  bool ok;
  int sp = command->parm("PERIOD_SHORT").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_SHORT" << command->parm("PERIOD_SHORT");
    return 1;
  }

  int mp = command->parm("PERIOD_MED").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_MED" << command->parm("PERIOD_MED");
    return 1;
  }

  int lp = command->parm("PERIOD_LONG").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD_LONG" << command->parm("PERIOD_LONG");
    return 1;
  }

  QList<Curve *> list;
  list << ihigh << ilow << iclose;
  line = getULTOSC(list, sp, mp, lp);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * ULTOSC::getULTOSC (QList<Curve *> &list, int sp, int mp, int lp)
{
  if (list.count() != 3)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &close[0], &close[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
			    sp,
			    mp,
			    lp,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getULTOSC: TA-Lib error" << rc;
    return 0;
  }

  QList<Curve *> outs;
  Curve *c = new Curve;
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

void ULTOSC::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME" << "INPUT_HIGH" << "INPUT_LOW" << "INPUT_CLOSE" << "PERIOD_SHORT" << "PERIOD_MED" << "PERIOD_LONG";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME", _plugin);
  set->setData("NAME:TYPE", QString("TEXT"));
  set->setData("INPUT_HIGH", QString("High"));
  set->setData("INPUT_HIGH:TYPE", QString("TEXT"));
  set->setData("INPUT_LOW", QString("Low"));
  set->setData("INPUT_LOW:TYPE", QString("TEXT"));
  set->setData("INPUT_CLOSE", QString("Close"));
  set->setData("INPUT_CLOSE:TYPE", QString("TEXT"));
  set->setData("PERIOD_SHORT", 7);
  set->setData("PERIOD_SHORT:TYPE", QString("INTEGER"));
  set->setData("PERIOD_MED", 14);
  set->setData("PERIOD_MED:TYPE", QString("INTEGER"));
  set->setData("PERIOD_LONG", 28);
  set->setData("PERIOD_LONG:TYPE", QString("INTEGER"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  ULTOSC *o = new ULTOSC;
  return ((Plugin *) o);
}
