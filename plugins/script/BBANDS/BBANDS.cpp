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
#include "Globals.h"
#include "MAType.h"
#include "InputType.h"

#include <QtDebug>

BBANDS::BBANDS ()
{
  _plugin = "BBANDS";
  _type = "INDICATOR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("BBANDS::BBANDS: error on TA_Initialize");
}

int BBANDS::command (Command *command)
{
  // PARMS:
  // INPUT
  // NAME_UPPER
  // NAME_MIDDLE
  // NAME_LOWER
  // PERIOD
  // MA_TYPE
  // DEV_UP
  // DEV_DOWN

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *input = i->line(command->parm("INPUT"));
  if (! input)
  {
    qDebug() << _plugin << "::command: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  QString uname = command->parm("NAME_UPPER");
  Curve *line = i->line(uname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_UPPER" << uname;
    return 1;
  }

  QString mname = command->parm("NAME_MIDDLE");
  line = i->line(mname);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME_MIDDLE" << mname;
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
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  if (input->count() < period)
    return 1;

  MAType types;
  int type = types.fromString(command->parm("MA_TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid MA_TYPE" << command->parm("MA_TYPE");
    return 1;
  }
  
  double udev = command->parm("DEV_UP").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid DEV_UP" << command->parm("DEV_UP");
    return 1;
  }

  double ldev = command->parm("DEV_DOWN").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid DEV_DOWN" << command->parm("DEV_DOWN");
    return 1;
  }

  QList<Curve *> list;
  list << input;
  QList<Curve *> lines = getBBANDS(list, period, udev, ldev, type);
  if (lines.count() != 3)
  {
    qDeleteAll(lines);
    return 1;
  }

  Curve *upper = lines.at(0);
  upper->setLabel(uname);
  i->setLine(uname, upper);
  
  Curve *middle = lines.at(1);
  middle->setLabel(mname);
  i->setLine(mname, middle);

  Curve *lower = lines.at(2);
  lower->setLabel(lname);
  i->setLine(lname, lower);

  command->setReturnCode("0");

  return 0;
}

QList<Curve *> BBANDS::getBBANDS (QList<Curve *> &list, int period, double udev, double ldev, int type)
{
  QList<Curve *> lines;
  if (! list.count())
    return lines;
  
  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return lines;

  int size = keys.count();
  TA_Real in[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &in[0], &in[0], &in[0], &in[0]);
  if (! size)
    return lines;

  TA_RetCode rc = TA_BBANDS(0,
                            size - 1,
                            &in[0],
                            period,
                            udev,
                            ldev,
                            (TA_MAType) type,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0],
			    &out3[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _plugin << "::getBBANDS: TA-Lib error" << rc;
    return lines;
  }

  Curve *c = new Curve;
  lines.append(c);
  c = new Curve;
  lines.append(c);
  c = new Curve;
  lines.append(c);
  if (it.outputs(lines, keys, outNb, &out[0], &out2[0], &out3[0]))
  {
    qDeleteAll(lines);
    lines.clear();
    return lines;
  }

  return lines;
}

void BBANDS::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME_UPPER"  << "NAME_MIDDLE" << "NAME_LOWER" << "INPUT" << "PERIOD" << "MA_TYPE" << "DEV_UP" << "DEV_DOWN";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME_UPPER", QString("BBU"));
  set->setData("NAME_UPPER:TYPE", QString("TEXT"));
  set->setData("NAME_MIDDLE", QString("BBM"));
  set->setData("NAME_MIDDLE:TYPE", QString("TEXT"));
  set->setData("NAME_LOWER", QString("BBL"));
  set->setData("NAME_LOWER:TYPE", QString("TEXT"));
  set->setData("INPUT", QString("Close"));
  set->setData("INPUT:TYPE", QString("TEXT"));
  set->setData("PERIOD", QString("14"));
  set->setData("PERIOD:TYPE", QString("INTEGER"));
  set->setData("DEV_UP", QString("2"));
  set->setData("DEV_UP:TYPE", QString("DOUBLE"));
  set->setData("DEV_DOWN", QString("2"));
  set->setData("DEV_DOWN:TYPE", QString("DOUBLE"));
  set->setData("MA_TYPE", QString("EMA"));
  set->setData("MA_TYPE:TYPE", QString("LIST"));
  MAType mat;
  set->setData("MA_TYPE:LIST", mat.list().join(","));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  BBANDS *o = new BBANDS;
  return ((Plugin *) o);
}
