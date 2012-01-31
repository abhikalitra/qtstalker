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

#include "CommandIndicator.h"
#include "Global.h"
#include "IndicatorFunctions.h"

#include <QtDebug>

CommandIndicator::CommandIndicator ()
{
  _name = "INDICATOR";
  _method << "LIST" << "ADD" << "REMOVE";
  
  Data td(_method, _method.at(0));
  Entity::set(QString("METHOD"), td);
  
  td = Data(TypeData::_FILE);
  Entity::set(QString("FILE"), td);
  
  td = Data(QString());
  Entity::set(QString("COMMAND"), td);
  
  td = Data(TypeData::_LIST);
  Entity::set(QString("LIST"), td);
}

QString CommandIndicator::run (CommandParse &, void *)
{
  Data method;
  Entity::toData(QString("METHOD"), method);
  
  switch (_method.indexOf(method.toString()))
  {
    case 0: // LIST
      if (list())
        return _returnCode;
      break;
    case 1: // ADD
      if (add())
        return _returnCode;
      break;
    case 2: // REMOVE
      if (remove())
        return _returnCode;
      break;
    default:
      qDebug() << "CommandIndicator::run: invalid method" << method.toString();
      return _returnCode;
      break;
  }
  
  _returnCode = "OK";
  return _returnCode;
}

int CommandIndicator::add ()
{
  Data file, command;
  Entity::toData(QString("FILE"), file);
  Entity::toData(QString("COMMAND"), command);
  
  QStringList l = file.toList();
  if (! l.size())
    return 1;
  
  QString ts = l.at(0);
  
  IndicatorFunctions func;
  if (func.add(command.toString(), ts))
    return 1;
  
  return 0;
}

int CommandIndicator::remove ()
{
  Data list;
  Entity::toData(QString("LIST"), list);
  
  IndicatorFunctions func;
  if (func.remove(list.toList()))
    return 1;
  
  return 0;
}

int CommandIndicator::list ()
{
  QStringList l;
  IndicatorFunctions func;
  if (func.list(l))
    return 1;

  Data list;
  Entity::toData(QString("LIST"), list);
  list.set(l);
  Entity::set(QString("LIST"), list);

  return 0;
}
