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

#include "CHART_OBJECT_DATABASE_TYPE.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>

CHART_OBJECT_DATABASE_TYPE::CHART_OBJECT_DATABASE_TYPE ()
{
  _plugin = "CHART_OBJECT_DATABASE_TYPE";
}

int CHART_OBJECT_DATABASE_TYPE::command (Command *command)
{
  // PARMS:
  // ID

  Setting co;
  co.setData("ID", command->parm("ID"));

  ChartObjectDataBase db;
  db.load(&co);

  command->setReturnData(_plugin + "_TYPE", co.data("Type"));

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_OBJECT_DATABASE_TYPE *o = new CHART_OBJECT_DATABASE_TYPE;
  return ((ScriptPlugin *) o);
}
