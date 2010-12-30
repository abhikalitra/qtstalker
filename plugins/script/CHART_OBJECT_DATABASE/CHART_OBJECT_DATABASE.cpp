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

#include "CHART_OBJECT_DATABASE.h"
#include "Indicator.h"

#include <QtDebug>

CHART_OBJECT_DATABASE::CHART_OBJECT_DATABASE ()
{
  _method << "IDS" << "TYPE" << "DELETE";
}

int CHART_OBJECT_DATABASE::command (Command *command)
{
  // CHART_OBJECT_DATABASE,<METHOD>
  //               0          1
  
  if (command->count() < 2)
  {
    qDebug() << "CHART_OBJECT_DATABASE::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _IDS:
      return ids(command);
      break;
    case _TYPE:
      return type(command);
      break;
    case _DELETE:
      return remove(command);
      break;
    default:
      break;
  }

  return 0;
}

int CHART_OBJECT_DATABASE::ids (Command *command)
{
  // CHART_OBJECT_DATABASE,IDS,INDICATOR,EXCHANGE,SYMBOL
  //            0           1      2        3       4

  if (command->count() != 5)
  {
    qDebug() << "CHART_OBJECT_DATABASE::ids: invalid parm count" << command->count();
    return 1;
  }

  BarData bd;
  bd.setExchange(command->parm(3));
  bd.setSymbol(command->parm(4));
  
  QStringList l;
  _db.ids(&bd, command->parm(2), l);

  command->setReturnData(l.join(","));

  return 0;
}

int CHART_OBJECT_DATABASE::type (Command *command)
{
  // CHART_OBJECT_DATABASE,TYPE,ID
  //            0           1    2

  if (command->count() != 3)
  {
    qDebug() << "CHART_OBJECT_DATABASE::ids: invalid parm count" << command->count();
    return 1;
  }

  Setting co;
  co.setData("ID", command->parm(2));

  _db.load(&co);

  command->setReturnData(co.data("Type"));

  return 0;
}

int CHART_OBJECT_DATABASE::remove (Command *command)
{
  // CHART_OBJECT_DATABASE,DELETE,ID*
  //            0            1    2*

  if (command->count() < 3)
  {
    qDebug() << "CHART_OBJECT_DATABASE::remove: invalid parm count" << command->count();
    return 1;
  }

  int loop = 2;
  for (; loop < command->count(); loop++)
    _db.deleteChartObject(command->parm(loop));

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_OBJECT_DATABASE *o = new CHART_OBJECT_DATABASE;
  return ((ScriptPlugin *) o);
}
