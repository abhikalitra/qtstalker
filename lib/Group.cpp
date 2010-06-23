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

void Group::setName (QString &d)
{
  _name = d;
}

int Group::deleteItem (int row)
{
  if (row < 0 || row >= _symbols.count())
    return 1;
  
  _symbols.removeAt(row);
  
  return 0;
}

int Group::getItem (int row, BarData &d)
{
  if (row < 0 || row >= _symbols.count())
    return 1;

  d = _symbols.at(row);
  return 0;
}

int Group::count ()
{
  return _symbols.count();
}

void Group::append (BarData &d)
{
  _symbols.append(d);
}

void Group::prepend (BarData &d)
{
  _symbols.prepend(d);
}

void Group::getStringList (QStringList &l)
{
  l.clear();
  
  int loop;
  for (loop = 0; loop < _symbols.count(); loop++)
  {
    BarData bd;
    getItem(loop, bd);
    l.append(bd.getExchange());
    l.append(bd.getSymbol());
  }
}

int Group::contains (BarData &d)
{
  int loop;
  for (loop = 0; loop < count(); loop++)
  {
    BarData bd;
    getItem(loop, bd);
    if (d.getSymbol() == bd.getSymbol())
    {
      if (d.getExchange() == bd.getExchange())
	return 1;
    }
  }
  
  return 0;
}

void Group::move (int from, int to)
{
  _symbols.move(from, to);
}

