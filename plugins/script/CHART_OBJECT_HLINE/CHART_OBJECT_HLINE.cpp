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

#include "CHART_OBJECT_HLINE.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>

CHART_OBJECT_HLINE::CHART_OBJECT_HLINE ()
{
}

int CHART_OBJECT_HLINE::command (Command *command)
{
  // CHART_OBJECT_HLINE,<NAME>,<INDICATOR>,<EXCHANGE>,<SYMBOL>,<PRICE>,<COLOR>
  //          0           1        2           3         4         5      6

  if (command->count() != 7)
  {
    qDebug() << "CHART_OBJECT_HLINE::command: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CHART_OBJECT_HLINE::command: no indicator";
    return 1;
  }

  int pos = 1;
  Setting co;
  co.setData("ID", command->parm(pos++));
  co.setData("Indicator", command->parm(pos++));
  co.setData("Exchange", command->parm(pos++));
  co.setData("Symbol", command->parm(pos++));

  // verify price
  bool ok;
  command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_HLINE::command: invalid price" << command->parm(pos);
    return 1;
  }
  co.setData("Price", command->parm(pos));
  pos++;

  // verify color
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "CHART_OBJECT_HLINE::command: invalid color" << command->parm(pos);
    return 1;
  }
  co.setData("Color", command->parm(pos));

  ChartObjectDataBase db;
  db.save(&co);

  i->addChartObject(co);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_OBJECT_HLINE *o = new CHART_OBJECT_HLINE;
  return ((ScriptPlugin *) o);
}
