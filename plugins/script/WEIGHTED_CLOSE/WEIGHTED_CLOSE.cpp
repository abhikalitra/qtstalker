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

#include "WEIGHTED_CLOSE.h"
#include "Globals.h"
#include "InputType.h"

#include <QtDebug>

WEIGHTED_CLOSE::WEIGHTED_CLOSE ()
{
  _plugin = "WEIGHTED_CLOSE";
  _type = "INDICATOR";
}

int WEIGHTED_CLOSE::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // INPUT_CLOSE
  // NAME

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
    qDebug() << _plugin << "::command: duplicate name" << name;
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

  QList<Curve *> list;
  list << ihigh << ilow << iclose;
  line = getWC(list);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * WEIGHTED_CLOSE::getWC (QList<Curve *> &list)
{
  if (list.count() != 3)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Curve *line = new Curve;
  Curve *ihigh = list.at(0);
  Curve *ilow = list.at(1);
  Curve *iclose = list.at(2);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *hbar = ihigh->bar(keys.at(loop));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(loop));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(loop));
    if (! cbar)
      continue;

    double t = (hbar->data() + lbar->data() + (cbar->data() * 2)) / 4.0;
    line->setBar(keys.at(loop), new CurveBar(t));
  }

  return line;
}

void WEIGHTED_CLOSE::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME" << "INPUT_HIGH" << "INPUT_LOW" << "INPUT_CLOSE";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME", QString("WC"));
  set->setData("NAME:TYPE", QString("TEXT"));
  set->setData("INPUT_HIGH", QString("High"));
  set->setData("INPUT_HIGH:TYPE", QString("TEXT"));
  set->setData("INPUT_LOW", QString("Low"));
  set->setData("INPUT_LOW:TYPE", QString("TEXT"));
  set->setData("INPUT_CLOSE", QString("Close"));
  set->setData("INPUT_CLOSE:TYPE", QString("TEXT"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  WEIGHTED_CLOSE *o = new WEIGHTED_CLOSE;
  return ((Plugin *) o);
}
