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

#include "FI.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"

#include <QtDebug>

FI::FI ()
{
  _plugin = "FI";
  _type = "INDICATOR";
}

int FI::command (Command *command)
{
  // PARMS:
  // INPUT_CLOSE
  // INPUT_VOLUME
  // NAME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  Curve *ivol = i->line(command->parm("INPUT_VOLUME"));
  if (! ivol)
  {
    qDebug() << _plugin << "::command: invalid INPUT_VOLUME" << command->parm("INPUT_VOLUME");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  QList<Curve *> list;
  list << iclose << ivol;
  line = getFI(list);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * FI::getFI (QList<Curve *> &list)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Curve *line = new Curve;
  Curve *close = list.at(0);
  Curve *vol = list.at(1);
  int loop = 1;
  double force = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *cbar = close->bar(keys.at(loop));
    if (! cbar)
      continue;

    CurveBar *ycbar = close->bar(keys.at(loop - 1));
    if (! ycbar)
      continue;

    CurveBar *vbar = vol->bar(keys.at(loop));
    if (! vbar)
      continue;

    double cdiff = cbar->data() - ycbar->data();
    force = vbar->data() * cdiff;

    line->setBar(keys.at(loop), new CurveBar(force));
  }

  return line;
}

void FI::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME" << "INPUT_VOLUME" << "INPUT_CLOSE";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME", _plugin);
  set->setData("NAME:TYPE", QString("TEXT"));
  set->setData("INPUT_VOLUME", QString("Volume"));
  set->setData("INPUT_VOLUME:TYPE", QString("TEXT"));
  set->setData("INPUT_CLOSE", QString("Close"));
  set->setData("INPUT_CLOSE:TYPE", QString("TEXT"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  FI *o = new FI;
  return ((Plugin *) o);
}
