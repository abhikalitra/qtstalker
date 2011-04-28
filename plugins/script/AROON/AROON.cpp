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
#include "Globals.h"
#include "InputType.h"

#include <QtDebug>

AROON::AROON ()
{
  _plugin = "AROON";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("AROON::AROON: error on TA_Initialize");
}

int AROON::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // NAME_UPPER
  // NAME_LOWER
  // PERIOD

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

  QString uname = command->parm("NAME_UPPER");
  Curve *line = i->line(uname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_UPPER" << uname;
    return 1;
  }

  QString lname = command->parm("NAME_LOWER");
  line = i->line(lname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_LOWER" << lname;
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid period" << command->parm("PERIOD");
    return 1;
  }

  QList<Curve *> list;
  list << ihigh << ilow;
  QList<Curve *> lines = getAROON(list, period);
  if (! lines.count())
    return 1;

  Curve *upper = lines.at(0);
  upper->setLabel(uname);
  i->setLine(uname, upper);

  if (lines.count() == 2)
  {
    Curve *lower = lines.at(1);
    lower->setLabel(lname);
    i->setLine(lname, lower);
  }

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> AROON::getAROON (QList<Curve *> &list, int period)
{
  QList<Curve *> lines;
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &low[0], &low[0]);
  if (! size)
    return lines;

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
    qDebug() << _plugin << "::getAROON: TA-Lib error" << rc;
    return lines;
  }

  Curve *c = new Curve;
  lines.append(c);
  c = new Curve;
  lines.append(c);
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out2[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

void AROON::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME_UPPER" << "NAME_LOWER" << "INPUT_HIGH" << "INPUT_LOW" << "PERIOD";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME_UPPER", QString("AROONU"));
  set->setData("NAME_UPPER:TYPE", QString("TEXT"));
  set->setData("NAME_LOWER", QString("AROONL"));
  set->setData("NAME_LOWER:TYPE", QString("TEXT"));
  set->setData("INPUT_HIGH", QString("High"));
  set->setData("INPUT_HIGH:TYPE", QString("TEXT"));
  set->setData("INPUT_LOW", QString("Low"));
  set->setData("INPUT_LOW:TYPE", QString("TEXT"));
  set->setData("PERIOD", QString("14"));
  set->setData("PERIOD:TYPE", QString("INTEGER"));
}
//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  AROON *o = new AROON;
  return ((Plugin *) o);
}
