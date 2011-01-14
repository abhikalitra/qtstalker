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

#include "INDICATOR_PLOT_INDEX_GET.h"

#include <QtDebug>

INDICATOR_PLOT_INDEX_GET::INDICATOR_PLOT_INDEX_GET ()
{
}

int INDICATOR_PLOT_INDEX_GET::command (Command *command)
{
  // INDICATOR_PLOT_INDEX_GET,<NAME>,<INDEX>
  //             0              1       2

  if (command->count() != 3)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_GET::command: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_GET::command: no indicator";
    return 1;
  }

  // verify name
  int pos = 1;
  Curve *line = i->line(command->parm(pos));
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_GET::command: name not found" << command->parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int index = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_GET::command: invalid index value" << command->parm(pos);
    return 1;
  }

  CurveBar *bar = line->bar(index);
  if (! bar)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_GET::command: bar not found" << command->parm(pos);
    return 1;
  }

  QString s = QString::number(bar->data());
  command->setReturnData(s);

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_PLOT_INDEX_GET *o = new INDICATOR_PLOT_INDEX_GET;
  return ((ScriptPlugin *) o);
}
