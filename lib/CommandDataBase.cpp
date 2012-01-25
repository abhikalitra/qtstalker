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

#include "CommandDataBase.h"
#include "EAVDataBase.h"
#include "TypeBarLength.h"
#include "TypeDateRange.h"

#include <QtDebug>

CommandDataBase::CommandDataBase ()
{
  _name = "DATABASE";
  _method << "LIST" << "LOAD" << "SAVE" << "REMOVE";
  
  Data td(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
  
  td = Data(QString());
  td.setLabel(QObject::tr("Table"));
  Entity::set(QString("TABLE"), td);
  
  td = Data(QString());
  td.setLabel(QObject::tr("Name"));
  Entity::set(QString("NAME"), td);
  
  td = Data(QString());
  Entity::set(QString("LIST"), td);
}

QString CommandDataBase::run (CommandParse &, void *)
{
  Data method;
  Entity::toData(QString("METHOD"), method);
  
  switch (_method.indexOf(method.toString()))
  {
    case 0: // LIST
      if (list())
        return _returnCode;
      break;
    case 1: // LOAD
      if (load())
        return _returnCode;
      break;
    case 2: // SAVE
      if (save())
        return _returnCode;
      break;
    case 3: // REMOVE
      if (remove())
        return _returnCode;
      break;
    default:
      qDebug() << "CommandDataBase::run: invalid method" << method.toString();
      return _returnCode;
      break;
  }
  
  _returnCode = "OK";
  return _returnCode;
}

int CommandDataBase::save ()
{
  Data name, table;
  Entity::toData(QString("NAME"), name);
  Entity::toData(QString("TABLE"), table);
  
  Entity record;
  record.setName(name.toString());
  
  QList<QString> keys = Entity::dkeys();
  
  QStringList skipKeys;
  skipKeys << "METHOD" << "TABLE" << "NAME" << "LIST";
  
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    if (skipKeys.indexOf(keys.at(loop)) != -1)
      continue;
    
    Data td;
    Entity::toData(keys.at(loop), td);
    record.set(keys.at(loop), td);
  }
  
  EAVDataBase db(table.toString());
  db.transaction();
  if (db.set(record))
  {
    qDebug() << "CommandDataBase::save: DataBase error" << name.toString();
    return 1;
  }
  db.commit();
  
  return 0;
}

int CommandDataBase::remove ()
{
  Data table, name;
  Entity::toData(QString("NAME"), name);
  Entity::toData(QString("TABLE"), table);

  QStringList tl;
  tl << name.toString();
  
  EAVDataBase db(table.toString());
  db.transaction();
  if (db.remove(tl))
  {
    qDebug() << "CommandDataBase::remove: DataBase error" << tl;
    return 1;
  }

  db.commit();
  
  return 0;
}

int CommandDataBase::list ()
{
  Data table;
  Entity::toData(QString("TABLE"), table);
  
  QStringList tl;
  EAVDataBase db(table.toString());
  if (db.names(tl))
  {
    qDebug() << "CommandDataBase::list: DataBase error";
    return 1;
  }

  Data list;
  Entity::toData(QString("LIST"), list);
  list.set(tl.join(";"));
  Entity::set(QString("LIST"), list);
  
  return 0;
}

int CommandDataBase::load ()
{
  Data table, name;
  Entity::toData(QString("TABLE"), table);
  Entity::toData(QString("NAME"), name);

  Entity record;
  record.setName(name.toString());
  
  EAVDataBase db(table.toString());
  if (db.get(record))
  {
    qDebug() << "CommandDataBase::load: DataBase error";
    return 1;
  }

  QList<QString> keys = record.dkeys();
qDebug() << "CommandDataBase::load:" << keys;

  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data td;
    record.toData(keys.at(loop), td);
    Entity::set(keys.at(loop), td);
  }
  
  return 0;
}

int CommandDataBase::setData (CommandParse &sg)
{
  // format object, setting, type, value...
  
  QStringList _extraTypes;
  _extraTypes << "LENGTH" << "RANGE";
  
  TypeData keys;
  int loop = 1; // skip past object name
  for (; loop < sg.values(); loop += 3)
  {
    int pos = loop;
    QString type = sg.value(pos++);
    QString settingName = sg.value(pos++);
    QString value = sg.value(pos);

    Data setting;
    if (Entity::toData(settingName, setting))
    {
      setting.setType((TypeData::Key) keys.stringToIndex(type));
      setting.setLabel(settingName);
    }
    
    // intercept a local type
    switch(_extraTypes.indexOf(type))
    {
      case 0: // LENGTH
      {
        TypeBarLength tbl;
	setting.setType(TypeData::_LIST);
	setting.set(tbl.list(), value);
        Entity::set(settingName, setting);
	continue;
        break;
      }
      case 1: // RANGE
      {
	TypeDateRange tdr;
	setting.setType(TypeData::_LIST);
	setting.set(tdr.list(), value);
        Entity::set(settingName, setting);
        break;
      }
      default:
      {
        // standard type
        switch ((TypeData::Key) setting.type())
        {
          case TypeData::_FILE:
	    setting.set(value.split(";"));
            break;
          default:
            if (setting.set(value, setting.type()))
            {
              qDebug() << "CommandDataBase::setData: invalid value" << setting.type() << settingName << type << value;
              return 1;
            }
            break;
        }
    
        Entity::set(settingName, setting);
	break;
      }
    }
  }

  return 0;
}
