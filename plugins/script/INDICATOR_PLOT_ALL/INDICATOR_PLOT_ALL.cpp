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

#include "INDICATOR_PLOT_ALL.h"

#include <QtDebug>

INDICATOR_PLOT_ALL::INDICATOR_PLOT_ALL ()
{
}

int INDICATOR_PLOT_ALL::command (Command *command)
{
  // INDICATOR_PLOT_ALL,<NAME>,<STYLE>,<COLOR>,<Z>
  //           0          1       2       3     4

  if (command->count() != 5)
  {
    qDebug() << "INDICATOR_PLOT_ALL::command: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_ALL::command: no indicator";
    return 1;
  }

  // verify name
  int pos = 1;
  QString name = command->parm(pos);

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_ALL::command: name not found" << name;
    return 1;
  }

  // verify style
  pos++;
  int style = line->typeFromString(command->parm(pos));
  if (style == -1)
  {
    qDebug() << "INDICATOR_PLOT_ALL::command: invalid style" << command->parm(pos);
    return 1;
  }
  line->setType((Curve::Type) style);

  // verify color
  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_ALL::setColorAll: invalid color" << command->parm(pos);
    return 1;
  }
  line->setAllColor(color);

  // verify Z
  pos++;
  bool ok;
  int z = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_ALL::command: invalid z value" << command->parm(pos);
    return 1;
  }

  if (z < 0)
  {
    qDebug() << "INDICATOR_PLOT_ALL::command: invalid z value must be > -1" << command->parm(pos);
    return 1;
  }

  line->setZ(z);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_PLOT_ALL *o = new INDICATOR_PLOT_ALL;
  return ((ScriptPlugin *) o);
}
