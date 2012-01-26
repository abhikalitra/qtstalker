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

#include "Entity.h"
#include "TypeEntity.h"

#include <QtDebug>

Entity::Entity ()
{
  _type = TypeEntity::_SETTING;
}

void Entity::setType (int d)
{
  _type = d;
}

int Entity::type ()
{
  return _type;
}

void Entity::setName (QString d)
{
  _name = d;
}

QString Entity::name ()
{
  return _name;
}

void Entity::remove (QString d)
{
  _data.remove(d);
}

void Entity::data (QHash<QString, Data> &d)
{
  d = _data;
}

int Entity::set (QString k, Data d)
{
  _data.insert(k, d);
  return 0;
}

int Entity::set (QHash<QString, Data> &d)
{
  _data = d;
  return 0;
}

int Entity::setEntity (int k, Entity &d)
{
  _bars.insert(k, d);
  return 0;
}

int Entity::toData (QString k, Data &d)
{
  int rc = 0;
  
  if (_data.contains(k))
    d = _data.value(k);
  else
    rc++;
  
  return rc;
}

int Entity::toIndex (int k, Entity &d)
{
  int rc = 0;
  
  if (_bars.contains(k))
    d = _bars.value(k);
  else
    rc++;
  
  return rc;
}

int Entity::toOffset (int k, Entity &d)
{
  if (k < 0)
    return 1;
  
  int count = k;
  QMapIterator<int, Entity> it(_bars);
  it.toBack();
  while (it.hasPrevious() && count > -1)
  {
    it.previous();
    count--;
  }
  
  if (! it.hasPrevious())
    return 1;
  
  d = it.value();  
  
  return 0;
}

int Entity::highLow (double &h, double &l)
{
  int rc = 1;
  int empty = 1;

  QHashIterator<QString, Data> it(_data);
  while (it.hasNext())
  {
    it.next();
    Data td = it.value();
    if (td.type() != TypeData::_DOUBLE)
      continue;

    double t = td.toDouble();
    
    if (empty)
    {
      empty = 0;
      h = t;
      l = t;
      rc = 0;
    }
    else
    {
      if (t > h)
        h = t;

      if (t < l)
        l = t;
    }
  }

  return rc;
}

void Entity::ekeys (QList<int> &d)
{
  d = _bars.keys();
}

void Entity::dkeys (QList<QString> &d)
{
  d = _data.keys();
}

int Entity::dkeyCount ()
{
  return _data.size();
}

int Entity::ekeyCount ()
{
  return _bars.size();
}

void Entity::ekeyRange (int &start, int &end)
{
  start = -1;
  end = -1;
  
  QMapIterator<int, Entity> it(_bars);
  it.toFront();
  if (it.hasNext())
  {
    it.next();
    start = it.key();
    end = start;
  }
  
  it.toBack();
  if (it.hasPrevious())
  {
    it.previous();
    end = it.key();
  }
}

void Entity::merge (Entity &osettings)
{
  QList<QString> keys;
  dkeys(keys);
  
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data ntd;
    toData(keys.at(loop), ntd);
    if (ntd.toString().isEmpty())
      continue;
      
    Data otd;
    if (osettings.toData(keys.at(loop), otd))
      continue;

    if (ntd.set(otd.toString(), ntd.type()))
      continue;
      
    set(keys.at(loop), ntd);
  }
}
