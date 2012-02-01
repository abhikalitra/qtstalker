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

#include "CommandSymbolDataBase.h"
#include "SymbolFunctions.h"

#include <QtDebug>

CommandSymbolDataBase::CommandSymbolDataBase ()
{
  _name = "SYMBOL_DATABASE";
  _method << "REMOVE";
  
  Data td(_method, _method.at(0));
  Entity::set(QString("METHOD"), td);
  
  td = Data(TypeData::_LIST);
  Entity::set(QString("LIST"), td);
}

QString CommandSymbolDataBase::run (CommandParse &, void *)
{
  Data method;
  Entity::toData(QString("METHOD"), method);
  
  switch (_method.indexOf(method.toString()))
  {
    case 0: // REMOVE
      if (remove())
        return _returnCode;
      break;
    default:
      qDebug() << "CommandSymbolDataBase::run: invalid method" << method.toString();
      return _returnCode;
      break;
  }
  
  _returnCode = "OK";
  return _returnCode;
}

int CommandSymbolDataBase::remove ()
{
  Data list;
  Entity::toData(QString("LIST"), list);

  SymbolFunctions sf;
  if (sf.remove(list.toList()))
  {
    qDebug() << "CommandSymbolDataBase::remove: DataBase error" << list.toList();
    return 1;
  }
  
  return 0;
}
