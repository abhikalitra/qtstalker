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

#include "INDICATOR_DATABASE.h"
#include "Globals.h"

#include <QtDebug>

INDICATOR_DATABASE::INDICATOR_DATABASE ()
{
  _method << "LOAD" << "SAVE" << "DELETE" << "INDICATORS";
}

int INDICATOR_DATABASE::command (Command *command)
{
  // INDICATOR_DATABASE,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "INDICATOR_DATABASE::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _LOAD:
      return load(command);
      break;
    case _SAVE:
      return save(command);
      break;
    case _DELETE:
      return deleteIndicator(command);
      break;
    case _INDICATORS:
      return indicators(command);
      break;
    default:
      break;
  }

  return 0;
}

int INDICATOR_DATABASE::load (Command *command)
{
  // INDICATOR_DATABASE,<LOAD>,<NAME>
  //          0            1       2

  if (command->count() != 3)
  {
    qDebug() << "INDICATOR_DATABASE::load: invalid parm count" << command->count();
    return 1;
  }

  Indicator i;
  i.setName(command->parm(2));
  
  if (_db.load(&i))
  {
    qDebug() << "INDICATOR_DATABASE::load: IndicatorDataBase error";
    return 1;
  }

  command->setReturnData(i.toString());

  return 0;
}

int INDICATOR_DATABASE::save (Command *command)
{
  // INDICATOR_DATABASE,<METHOD>,<NAME>,<COMMAND>,<SCRIPT>,<ROW>,<LOG>,<DATE>
  //           0           1       2        3        4       5     6     7

  if (command->count() != 8)
  {
    qDebug() << "INDICATOR_DATABASE::save: invalid parm count" << command->count();
    return 1;
  }

  int pos = 2;
  Indicator i;
  i.setName(command->parm(pos++));
  i.setCommand(command->parm(pos++));
  i.setScript(command->parm(pos++));

  bool ok;
  int t = command->parm(pos++).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_DATABASE::save: invalid row" << t;
    return 1;
  }
  i.setLock(t);
  
  t = command->parm(pos++).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_DATABASE::save: invalid log" << t;
    return 1;
  }
  i.setLog(t);

  t = command->parm(pos++).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_DATABASE::save: invalid date" << t;
    return 1;
  }
  i.setDate(t);

  if (_db.save(&i))
  {
    qDebug() << "INDICATOR_DATABASE::save: IndicatorDataBase error";
    return 1;
  }

  command->setReturnData("0");

  return 0;
}

int INDICATOR_DATABASE::deleteIndicator (Command *command)
{
  // INDICATOR_DATABASE,<DELETE>,<NAME>,*
  //           0           1       2

  if (command->count() < 3)
  {
    qDebug() << "INDICATOR_DATABASE::deleteIndicator: invalid parm count" << command->count();
    return 1;
  }

  QStringList l;
  int pos = 2;
  for (; pos < command->count(); pos++)
    l << command->parm(pos);

  if (_db.deleteIndicator(l))
  {
    qDebug() << "INDICATOR_DATABASE::deleteIndicator: IndicatorDataBase error";
    return 1;
  }
    
  command->setReturnData("0");
  
  return 0;
}

int INDICATOR_DATABASE::indicators (Command *command)
{
  // INDICATOR_DATABASE,INDICATORS
  //           0           1

  QStringList l;
  if (_db.indicators(l))
  {
    qDebug() << "INDICATOR_DATABASE::indicators: IndicatorDataBase error";
    return 1;
  }
  
  command->setReturnData(l.join(","));

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_DATABASE *o = new INDICATOR_DATABASE;
  return ((ScriptPlugin *) o);
}
