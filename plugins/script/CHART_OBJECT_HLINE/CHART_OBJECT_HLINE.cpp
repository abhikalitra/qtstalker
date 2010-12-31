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
  _method << "RO" << "RW";
}

int CHART_OBJECT_HLINE::command (Command *command)
{
  // CHART_OBJECT_HLINE,<METHOD>
  //          0           1

  if (command->count() < 2)
  {
    qDebug() << "CHART_OBJECT_HLINE::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _RO:
      return createRO(command);
      break;
    case _RW:
      return createRW(command);
      break;
    default:
      break;
  }

  return 0;
}

int CHART_OBJECT_HLINE::createRW (Command *command)
{
  // CHART_OBJECT_HLINE,<METHOD>,<NAME>,<INDICATOR>,<EXCHANGE>,<SYMBOL>,<PRICE>,<COLOR>
  //          0            1        2         3         4         5       6        7

  if (command->count() != 8)
  {
    qDebug() << "CHART_OBJECT_HLINE::createRW: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CHART_OBJECT_HLINE::createRW: no indicator";
    return 1;
  }

  int pos = 2;
  Setting co;
  co.setData("Type", QString("HLine"));
  co.setData("ID", command->parm(pos++));
  co.setData("Indicator", command->parm(pos++));
  co.setData("Exchange", command->parm(pos++));
  co.setData("Symbol", command->parm(pos++));

  // verify price
  bool ok;
  command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_HLINE::createRW: invalid price" << command->parm(pos);
    return 1;
  }
  co.setData("Price", command->parm(pos++));

  // verify color
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "CHART_OBJECT_HLINE::createRW: invalid color" << command->parm(pos);
    return 1;
  }
  co.setData("Color", command->parm(pos++));

  ChartObjectDataBase db;
  db.save(&co);

  i->addChartObject(co);

  command->setReturnData("0");

  return 0;
}

int CHART_OBJECT_HLINE::createRO (Command *command)
{
  // CHART_OBJECT_HLINE,<METHOD>,<PRICE>,<COLOR>
  //          0            1        2       3 

  if (command->count() != 4)
  {
    qDebug() << "CHART_OBJECT_HLINE::createRO: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CHART_OBJECT_HLINE::createRO: no indicator";
    return 1;
  }

  Setting co;
  QString key = "-" + QString::number(i->chartObjectCount() + 1);
  co.setData("Type", QString("HLine"));
  co.setData("ID", key);
  co.setData("RO", 1);

  // verify price
  int pos = 2;
  bool ok;
  command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_HLINE::createRO: invalid price" << command->parm(pos);
    return 1;
  }
  co.setData("Price", command->parm(pos++));

  // verify color
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "CHART_OBJECT_HLINE::createRO: invalid color" << command->parm(pos);
    return 1;
  }
  co.setData("Color", command->parm(pos++));

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
