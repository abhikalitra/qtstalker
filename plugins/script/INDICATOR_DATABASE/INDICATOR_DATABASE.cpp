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
  _plugin = "INDICATOR_DATABASE";
//  _method << "LOAD" << "SAVE" << "DELETE" << "INDICATORS" << "DIALOG";
  _method << "LOAD" << "SAVE" << "DELETE" << "INDICATORS";
}

int INDICATOR_DATABASE::command (Command *command)
{
  // PARMS:
  // METHOD

  switch ((Method) _method.indexOf(command->parm("METHOD")))
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
//    case _DIALOG:
//      return dialog(command);
//      break;
    default:
      break;
  }

  return 0;
}

int INDICATOR_DATABASE::load (Command *command)
{
  // PARMS:
  // METHOD (LOAD)
  // NAME

  Indicator i;
  i.setName(command->parm("NAME"));
  
  if (_db.load(&i))
  {
    qDebug() << _plugin << "::load: IndicatorDataBase error";
    return 1;
  }

//  command->setReturnData(_plugin + "_INDICATOR", i.toString());

  command->setReturnCode("0");

  return 0;
}

int INDICATOR_DATABASE::save (Command *command)
{
  // PARMS:
  // METHOD
  // NAME
  // COMMAND
  // SCRIPT
  // ROW
  // LOG
  // DATE

  Indicator i;
  i.setName(command->parm("NAME"));
  i.setCommand(command->parm("COMMAND"));
  i.setScript(command->parm("SCRIPT"));

  bool ok;
  int t = command->parm("ROW").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::save: invalid row" << t;
    return 1;
  }
  i.setLock(t);
  
  t = command->parm("LOG").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::save: invalid log" << t;
    return 1;
  }
  i.setLog(t);

  t = command->parm("DATE").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::save: invalid date" << t;
    return 1;
  }
  i.setDate(t);

  if (_db.save(&i))
  {
    qDebug() << _plugin << "::save: IndicatorDataBase error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int INDICATOR_DATABASE::deleteIndicator (Command *command)
{
  // PARMS:
  // METHOD (DELETE)
  // NAME

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  if (_db.deleteIndicator(l))
  {
    qDebug() << _plugin << "::deleteIndicator: IndicatorDataBase error";
    return 1;
  }
    
  command->setReturnCode("0");
  
  return 0;
}

int INDICATOR_DATABASE::indicators (Command *command)
{
  // PARMS:
  // INDICATORS

  QStringList l;
  if (_db.indicators(l))
  {
    qDebug() << _plugin << "::indicators: IndicatorDataBase error";
    return 1;
  }
  
  command->setReturnData(_plugin + "_INDICATORS", l.join(";"));

  command->setReturnCode("0");

  return 0;
}

/*
int INDICATOR_DATABASE::dialog (Command *command)
{
  // PARMS:
  // DIALOG
  // NAME

  QString d;
  if (_db.dialog(command->parm("NAME"), d))
  {
    qDebug() << _plugin << "::dialog: IndicatorDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "", d);

  command->setReturnCode("0");

  return 0;
}
*/

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_DATABASE *o = new INDICATOR_DATABASE;
  return ((ScriptPlugin *) o);
}
