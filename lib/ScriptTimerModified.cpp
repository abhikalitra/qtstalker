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

#include "ScriptTimerModified.h"
#include "GlobalScriptTimer.h"
#include "EAVDataBase.h"
#include "KeyScriptDataBase.h"
#include "ScriptTimerAdd.h"

#include <QtDebug>

ScriptTimerModified::ScriptTimerModified ()
{
}

int ScriptTimerModified::modified (QString d)
{
  KeyScriptDataBase keys;
  EAVDataBase db("scripts");

  Entity data;
  data.setName(d);

  if (db.get(data))
    return 1;

  Data interval;
  data.toData(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  ScriptTimer *st = g_scriptTimerList.value(d);
  if (! st)
  {
    if (interval.toInteger() > 0)
    {
      ScriptTimerAdd sta;
      sta.add(data);
    }
  }
  else
  {
    st->stop();
    
    if (interval.toInteger() == 0)
    {
      g_scriptTimerList.remove(d);
      delete st;
    }
    else
    {
      Data file, command;
      data.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
      data.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
      
      st->setFile(file.toString());
      st->setCommand(command.toString());
      st->setIntervalString(interval.toString());
      
      st->start();
    }
  }
  
  return 0;
}
