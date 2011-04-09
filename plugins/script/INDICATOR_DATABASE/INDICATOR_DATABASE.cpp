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
#include "DataDataBase.h"

#include <QtDebug>

INDICATOR_DATABASE::INDICATOR_DATABASE ()
{
  _plugin = "INDICATOR_DATABASE";
  _method << "DELETE" << "LIST" << "DUMP" << "IMPORT";
}

int INDICATOR_DATABASE::command (Command *command)
{
  int rc = 1;
  switch (_method.indexOf(command->parm("METHOD")))
  {
    case 0:
      rc = remove(command);
      break;
    case 1:
      rc = list(command);
      break;
    case 2:
      rc = dump(command);
      break;
    case 3:
      rc = import(command);
      break;
    default:
      break;
  }

  return rc;
}

int INDICATOR_DATABASE::remove (Command *command)
{
  // PARMS:
  // NAME

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  Indicator i;
  i.remove(1, l);
    
  command->setReturnCode("0");
  
  return 0;
}

int INDICATOR_DATABASE::list (Command *command)
{
  // PARMS:

  QStringList l;
  DataDataBase db("indicators");
  if (db.names(l))
  {
    qDebug() << _plugin << "::list: IndicatorDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_INDICATORS", l.join(";"));

  command->setReturnCode("0");

  return 0;
}

int INDICATOR_DATABASE::dump (Command *command)
{
  // PARMS:
  // NAME - semicolon delimited string
  // DIRECTORY - place to dump indicator files

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);
  
  QString path = command->parm("DIRECTORY");
  
  DataDataBase db("indicators");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString s = path + "/" + l.at(loop);
    if (db.dumpName(l.at(loop), s))
      qDebug() << _plugin << "::dump: IndicatorDataBase error";
  }

  command->setReturnCode("0");

  return 0;
}

int INDICATOR_DATABASE::import (Command *command)
{
  // PARMS:
  // NAME - semicolon delimited string

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  DataDataBase db("indicators");
  db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QFileInfo fi(l.at(loop));
    if (db.importName(fi.fileName(), l.at(loop)))
      qDebug() << _plugin << "::import: IndicatorDataBase error";
  }

  db.commit();

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  INDICATOR_DATABASE *o = new INDICATOR_DATABASE;
  return ((Plugin *) o);
}

