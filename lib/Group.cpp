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

#include "Group.h"

Group::Group ()
{
}

void Group::clear ()
{
  _symbols.clear();
}

QString & Group::getName ()
{
  return _name;
}

void Group::setName (QString d)
{
  _name = d;
}

void Group::deleteSymbol (QString key)
{
  _symbols.remove(key);
}

int Group::getSymbol (QString key, BarData &d)
{
  if (! _symbols.contains(key))
    return 1;

  d = _symbols.value(key);
  return 0;
}

int Group::count ()
{
  return _symbols.count();
}

void Group::setSymbol (BarData d)
{
  QString k = d.getKey();
  _symbols.insert(k, d);
}

void Group::getStringList (QStringList &l)
{
  l.clear();

  QHashIterator<QString, BarData> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    
    BarData bd = it.value();
    l.append(bd.getKey());
  }
}

int Group::contains (BarData d)
{
  QString k = d.getKey();
  return _symbols.contains(k);
}

