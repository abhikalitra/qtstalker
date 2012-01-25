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

#include "CommandGroupDataBase.h"
#include "EAVDataBase.h"
#include "KeyGroupDataBase.h"

#include <QtDebug>

CommandGroupDataBase::CommandGroupDataBase ()
{
  _name = "GROUP_DATABASE";
  _method << "LIST" << "SAVE" << "REMOVE";
  
  Data td(_method, _method.at(0));
  Entity::set(QString("METHOD"), td);
  
  td = Data(QString());
  Entity::set(QString("NAME"), td);
  
  td = Data(QString());
  Entity::set(QString("LIST"), td);
}

QString CommandGroupDataBase::run (CommandParse &, void *)
{
  Data method;
  Entity::toData(QString("METHOD"), method);
  
  switch (_method.indexOf(method.toString()))
  {
    case 0: // LIST
      if (list())
        return _returnCode;
      break;
    case 1: // SAVE
      if (save())
        return _returnCode;
      break;
    case 2: // REMOVE
      if (remove())
        return _returnCode;
      break;
    default:
      qDebug() << "CommandGroupDataBase::run: invalid method" << method.toString();
      return _returnCode;
      break;
  }
  
  _returnCode = "OK";
  return _returnCode;
}

int CommandGroupDataBase::save ()
{
  Data name, list;
  Entity::toData(QString("NAME"), name);
  Entity::toData(QString("LIST"), list);

  KeyGroupDataBase keys;
  Entity data;
  data.setName(name.toString());
  data.set(keys.indexToString(KeyGroupDataBase::_LIST), list);
  
  EAVDataBase db("groups");
  db.transaction();
  if (db.set(data))
  {
    qDebug() << "CommandGroupDataBase::save: DataBase error" << name.toString();
    return 1;
  }
  db.commit();
  
  return 0;
}

int CommandGroupDataBase::remove ()
{
  Data name;
  Entity::toData(QString("NAME"), name);

  QStringList tl;
  tl << name.toString();
  
  EAVDataBase db("groups");
  db.transaction();
  if (db.remove(tl))
  {
    qDebug() << "CommandGroupDataBase::remove: DataBase error" << tl;
    return 1;
  }
  db.commit();
  
  return 0;
}

int CommandGroupDataBase::list ()
{
  QStringList tl;
  EAVDataBase db("groupss");
  if (db.names(tl))
  {
    qDebug() << "CommandGroupDataBase::list: DataBase error";
    return 1;
  }

  Data list;
  Entity::toData(QString("LIST"), list);
  list.set(tl.join(";"));
  Entity::set(QString("LIST"), list);
  
  return 0;
}
