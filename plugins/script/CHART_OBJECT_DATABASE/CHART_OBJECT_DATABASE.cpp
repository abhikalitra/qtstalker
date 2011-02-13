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
  _plugin = "CHART_OBJECT_DATABASE";
  _method << "IDS" << "TYPE" << "DELETE";
}

int CHART_OBJECT_DATABASE::command (Command *command)
{
  // PARMS:
  // METHOD

  switch ((Method) _method.indexOf(command->parm("METHOD")))
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
  // PARMS:
  // METHOD (IDS)
  // INDICATOR
  // EXCHANGE
  // SYMBOL

  BarData bd;
  bd.setExchange(command->parm("EXCHANGE"));
  bd.setSymbol(command->parm("SYMBOL"));
  
  QStringList l;
  _db.ids(&bd, command->parm("INDICATOR"), l);

  command->setReturnData(_plugin + "_IDS", l.join(","));

  command->setReturnCode("0");

  return 0;
}

int CHART_OBJECT_DATABASE::type (Command *command)
{
  // PARMS:
  // METHOD (TYPE)
  // ID

  Setting co;
  co.setData("ID", command->parm("ID"));

  _db.load(&co);

  command->setReturnData(_plugin + "_TYPE", co.data("Type"));

  command->setReturnCode("0");

  return 0;
}

int CHART_OBJECT_DATABASE::remove (Command *command)
{
  // PARMS:
  // METHOD (DELETE)
  // ID

  _db.deleteChartObject(command->parm("ID"));

  command->setReturnCode("0");

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
