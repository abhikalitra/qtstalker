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

#include "NORMALIZE.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"
#include "NORMALIZEWidget.h"

#include <QtDebug>
#include <cmath>

NORMALIZE::NORMALIZE ()
{
  _plugin = "NORMALIZE";
  _type = "INDICATOR";
}

int NORMALIZE::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT

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

  QList<Curve *> list;
  list << in;
  line = getNORM(list);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * NORMALIZE::getNORM (QList<Curve *> &list)
{
  if (! list.count())
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Curve *in = list.at(0);
  double max = 0;
  double min = 0;
  if (in->highLow(max, min))
    return 0;
    
  double range = fabs(max) + fabs(min);
  
  Curve *line = new Curve;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    line->setBar(keys.at(loop), new CurveBar(((bar->data() - min) / range) * 100));
  }

  return line;
}

void NORMALIZE::settings (Setting *set)
{
  set->clear();

  QStringList keys;
  keys << "NAME" << "INPUT";
  set->setData("KEYS", keys.join(","));

  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME", _plugin);
  set->setData("NAME:TYPE", QString("TEXT"));
  set->setData("INPUT", QString("Close"));
  set->setData("INPUT:TYPE", QString("TEXT"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  NORMALIZE *o = new NORMALIZE;
  return ((Plugin *) o);
}
