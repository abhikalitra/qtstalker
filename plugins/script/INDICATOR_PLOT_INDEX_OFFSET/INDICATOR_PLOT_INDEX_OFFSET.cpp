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

#include "INDICATOR_PLOT_INDEX_OFFSET.h"

#include <QtDebug>

INDICATOR_PLOT_INDEX_OFFSET::INDICATOR_PLOT_INDEX_OFFSET ()
{
}

int INDICATOR_PLOT_INDEX_OFFSET::command (Command *command)
{
  // INDICATOR_PLOT_INDEX_OFFSET,<INDEX>
  //               0                1

  if (command->count() != 2)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_OFFSET::command: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_OFFSET::command: no indicator";
    return 1;
  }

  // verify index
  int pos = 1;
  QStringList l = command->parm(pos).split(".", QString::SkipEmptyParts);
  if (l.count() != 2)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_OFFSET::command: invalid index syntax" << command->parm(pos);
    return 1;
  }

  Curve *line = i->line(l.at(0));
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_OFFSET::command: name not found" << command->parm(pos);
    return 1;
  }

  bool ok;
  int index = l.at(1).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_OFFSET::command: invalid index value" << command->parm(pos);
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);
  CurveBar *bar = line->bar(end - index);
  if (! bar)
  {
    qDebug() << "INDICATOR_PLOT_INDEX_OFFSET::command: bar not found" << command->parm(pos);
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
  INDICATOR_PLOT_INDEX_OFFSET *o = new INDICATOR_PLOT_INDEX_OFFSET;
  return ((ScriptPlugin *) o);
}
