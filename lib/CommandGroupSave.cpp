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

#include "CommandGroupSave.h"
#include "EAVDataBase.h"
#include "GroupDataBaseKey.h"

#include <QtDebug>

CommandGroupSave::CommandGroupSave ()
{
  _name = "GROUP_SAVE";

  Data td;
  td.setLabel(QObject::tr("Group"));
  Entity::set(QString("GROUP"), td);
  
  td = Data(QStringList());
  td.setLabel(QObject::tr("Symbols"));
  Entity::set(QString("SYMBOLS"), td);
}

QString CommandGroupSave::run (CommandParse &, void *)
{
  Data group, symbols;
  Entity::toData(QString("GROUP"), group);
  Entity::toData(QString("SYMBOLS"), symbols);
  
  GroupDataBaseKey keys;
  Entity i;
  i.setName(group.toString());
  i.set(keys.indexToString(GroupDataBaseKey::_LIST), symbols);
  
  EAVDataBase db("groups");
  db.transaction();
  if (db.set(&i))
  {
    qDebug() << "CommandGroupSave::run: EAVDataBase error";
    return _returnCode;
  }
  db.commit();

  _returnCode = "OK";
  return _returnCode;
}
