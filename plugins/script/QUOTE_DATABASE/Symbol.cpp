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

#include "Symbol.h"

#include <QtDebug>

Symbol::Symbol ()
{
}

Symbol::~Symbol ()
{
  clear();
}

void Symbol::clear ()
{
  if (_list.count())
    qDeleteAll(_list);
}

int Symbol::count ()
{
  return (int) _list.count();
}

void Symbol::prepend (QuoteBar *bar)
{
  _list.prepend(bar);
}

void Symbol::append (QuoteBar *bar)
{
  _list.append(bar);
}

QuoteBar * Symbol::bar (int d)
{
  return _list.at(d);
}

QString & Symbol::symbol ()
{
  return _symbol;
}

void Symbol::setSymbol (QString d)
{
  _symbol = d;
}

QString & Symbol::name ()
{
  return _name;
}

void Symbol::setName (QString d)
{
  _name = d;
}

QString & Symbol::exchange ()
{
  return _exchange;
}

void Symbol::setExchange (QString d)
{
  _exchange = d;
}

QString & Symbol::type ()
{
  return _type;
}

void Symbol::setType (QString d)
{
  _type = d;
}

QString & Symbol::table ()
{
  return _table;
}

void Symbol::setTable (QString d)
{
  _table = d;
}
