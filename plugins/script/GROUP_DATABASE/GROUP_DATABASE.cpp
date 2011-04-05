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

#include "GROUP_DATABASE.h"
#include "DataDataBase.h"

#include <QtDebug>

GROUP_DATABASE::GROUP_DATABASE ()
{
  _plugin = "GROUP_DATABASE";
  _method << "DELETE" << "LIST" << "LOAD" << "SAVE" << "SAVE_ALL";
}

int GROUP_DATABASE::command (Command *command)
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
      rc = load(command);
      break;
    case 3:
      rc = save(command);
      break;
    case 4:
      rc = saveAll(command);
      break;
    default:
      break;
  }

  return rc;
}

int GROUP_DATABASE::remove (Command *command)
{
  // PARMS:
  // NAME - semicolon delimited string

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  DataDataBase db("groups");
  db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
    db.removeName(l.at(loop));

  db.commit();

  command->setReturnCode("0");

  return 0;
}

int GROUP_DATABASE::list (Command *command)
{
  // PARMS:

  DataDataBase db("groups");
  QStringList l;
  if (db.names(l))
  {
    qDebug() << _plugin << "::list: DataDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_GROUPS", l.join(";"));

  command->setReturnCode("0");

  return 0;
}

int GROUP_DATABASE::load (Command *command)
{
  // PARMS:
  // NAME

  QString name = command->parm("NAME");

  DataDataBase db("groups");
  QStringList l;
  if (db.load(name, l))
  {
    qDebug() << _plugin << "::load: DataDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_GROUP", l.join(";"));

  command->setReturnCode("0");

  return 0;
}

int GROUP_DATABASE::save (Command *command)
{
  // PARMS:
  // NAME
  // ITEMS - semicolon delimited string

  QString name = command->parm("NAME");

  QStringList l = command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  DataDataBase db("groups");
  QStringList g;
  db.load(name, g);

  int loop = 0;
  for (; loop < l.count(); loop++)
    g << l.at(loop);

  g.removeDuplicates();

  db.transaction();
  db.removeName(name);
  db.save(name, g);
  db.commit();

  command->setReturnCode("0");

  return 0;
}

int GROUP_DATABASE::saveAll (Command *command)
{
  // PARMS:
  // NAME
  // ITEMS -  - semicolon delimited string

  QString name = command->parm("NAME");

  QStringList l = command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  DataDataBase db("groups");
  db.transaction();
  db.removeName(name);
  if (db.save(name, l))
  {
    qDebug() << _plugin << "::saveAll: GroupDataBase error";
    return 1;
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
  GROUP_DATABASE *o = new GROUP_DATABASE;
  return ((Plugin *) o);
}
