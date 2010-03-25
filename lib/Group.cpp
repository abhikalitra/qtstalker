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

Group::~Group ()
{
  clear();
}

void Group::clear ()
{
  qDeleteAll(symbols);
  symbols.clear();
}

QString & Group::getName ()
{
  return name;
}

void Group::setName (QString &d)
{
  name = d;
}

int Group::deleteItem (int row)
{
  int rc = 1;
  BarData *bd = symbols.at(row);
  if (bd)
  {
    delete bd;
    symbols.removeAt(row);
    rc = 0;
  }
  
  return rc;
}

BarData * Group::getItem (int row)
{
  return symbols.at(row);
}

int Group::count ()
{
  return symbols.count();
}

void Group::append (BarData *d)
{
  BarData *bd = new BarData;
  bd->setExchange(d->getExchange());
  bd->setSymbol(d->getSymbol());
  bd->setName(d->getName());
  symbols.append(bd);
}

