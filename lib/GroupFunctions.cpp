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

#include "GroupFunctions.h"
#include "KeyGroupDataBase.h"
#include "Entity.h"

#include <QtDebug>

GroupFunctions::GroupFunctions ()
{
  _db = EAVDataBase("groups");
}

int GroupFunctions::add (QString name)
{
  Entity g;
  g.setName(name);
  
  if (_db.get(g))
  {
    qDebug() << "GroupFunctions::add: invalid name" << name;
    return 1;
  }

  KeyGroupDataBase gkeys;
  Data td;
  g.toData(gkeys.indexToString(KeyGroupDataBase::_LIST), td);
  QStringList l = td.toString().split(";", QString::SkipEmptyParts);

  l << name;
  l.removeDuplicates();

  td.set(l.join(";"));
  g.set(gkeys.indexToString(KeyGroupDataBase::_LIST), td);

  _db.transaction();
  if (_db.set(g))
  {
    qDebug() << "GroupFunctions::add: database error";
    return 1;
  }
  _db.commit();

  return 0;
}

int GroupFunctions::remove (QStringList gl)
{
  if (! gl.count())
  {
    qDebug() << "GroupFunctions::remove: empty list";
    return 1;
  }

  _db.transaction();
  if (_db.remove(gl))
  {
    qDebug() << "GroupFunctions::remove: database error";
    return 1;
  }
  _db.commit();

  return 0;
}

int GroupFunctions::names (QStringList &l)
{
  if (_db.names(l))
  {
    qDebug() << "GroupFunctions::names: database error";
    return 1;
  }
  
  return 0;
}

int GroupFunctions::get (Entity &group)
{
  if (_db.get(group))
  {
    qDebug() << "GroupFunctions::get: database error";
    return 1;
  }
  
  return 0;
}

int GroupFunctions::set (Entity &group)
{
  _db.transaction();
  if (_db.set(group))
  {
    qDebug() << "GroupFunctions::set: database error";
    return 1;
  }
  _db.commit();
  
  return 0;
}
