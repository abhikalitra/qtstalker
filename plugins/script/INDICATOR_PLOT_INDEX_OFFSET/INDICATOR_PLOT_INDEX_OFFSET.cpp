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
  _plugin = "INDICATOR_PLOT_INDEX_OFFSET";
}

int INDICATOR_PLOT_INDEX_OFFSET::command (Command *command)
{
  // PARMS:
  // INDEX

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify index
  QStringList l = command->parm("INDEX").split(".", QString::SkipEmptyParts);
  if (l.count() != 2)
  {
    qDebug() << _plugin << "::command: invalid INDEX" << command->parm("INDEX");
    return 1;
  }

  Curve *line = i->line(l.at(0));
  if (! line)
  {
    qDebug() << _plugin << "::command: name not found" << command->parm("INDEX");
    return 1;
  }

  bool ok;
  int index = l.at(1).toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid INDEX value" << command->parm("INDEX");
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);
  CurveBar *bar = line->bar(end - index);
  if (! bar)
  {
    qDebug() << _plugin << "::command: bar not found at index" << index;
    return 1;
  }

  QString s = QString::number(bar->data());
  command->setReturnData(_plugin + "_VALUE", s);

  command->setReturnCode("0");

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
