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
#include "CurveBar.h"

#include <QtDebug>

Symbol::Symbol ()
{
  clear();
}

Symbol::~Symbol ()
{
  clear();
}

void Symbol::clear ()
{
  _startIndex = 99999999;
  _endIndex = -99999999;
  _symbol.clear();
  _exchange.clear();
  _name.clear();
  _length = -1;
  _range = -1;
  _startDate = QDateTime();
  _endDate = QDateTime();
  _table.clear();
  _type = "Stock";

  qDeleteAll(_bars);
  _bars.clear();
}

int Symbol::set (int k, Data *d)
{
  Data *b = _bars.value(k);
  if (b)
    delete b;

  _bars.insert(k, d);

  if (k < _startIndex)
    _startIndex = k;
  if (k > _endIndex)
    _endIndex = k;

  return 0;
}

Data * Symbol::getData (int d)
{
  return _bars.value(d);
}

QList<int> Symbol::barKeys ()
{
  return _bars.keys();
}

int Symbol::barKeyCount ()
{
  return _bars.count();
}

void Symbol::append (Data *d)
{
  int i = _endIndex + 1;
  set(i, d);
}

void Symbol::setSymbol (QString d)
{
  _symbol = d;
}

QString Symbol::symbol ()
{
  return _symbol;
}

void Symbol::setExchange (QString d)
{
  _exchange = d;
}

QString Symbol::exchange ()
{
  return _exchange;
}

void Symbol::setName (QString d)
{
  _name = d;
}

QString Symbol::name ()
{
  return _name;
}

void Symbol::setLength (int d)
{
  _length = d;
}

int Symbol::length ()
{
  return _length;
}

void Symbol::setRange (int d)
{
  _range = d;
}

int Symbol::range ()
{
  return _range;
}

void Symbol::setStartDate (QDateTime d)
{
  _startDate = d;
}

QDateTime Symbol::startDate ()
{
  return _startDate;
}

void Symbol::setEndDate (QDateTime d)
{
  _endDate = d;
}

QDateTime Symbol::endDate ()
{
  return _endDate;
}

void Symbol::setTable (QString d)
{
  _table = d;
}

QString Symbol::table ()
{
  return _table;
}

void Symbol::setType (QString d)
{
  _type = d;
}

QString Symbol::type ()
{
  return _type;
}

void Symbol::copy (Symbol *d)
{
  d->clear();

  d->setSymbol(symbol());
  d->setExchange(exchange());
  d->setName(name());
  d->setLength(length());
  d->setRange(range());
  d->setStartDate(startDate());
  d->setEndDate(endDate());
  d->setTable(table());
  d->setType(type());

  QList<int> keys = barKeys();
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
     Data *obar = getData(keys.at(loop));
     Data *nbar = new CurveBar;
     nbar->fromSaveString(obar->toString());
     d->set(keys.at(loop), nbar);
  }
}

QString Symbol::symbolFull ()
{
  QString s = _exchange + ":" + _symbol;
  return s;
}

int Symbol::setSymbolFull (QString d)
{
  QStringList l = d.split(":");
  if (l.count() != 2)
    return 1;

  _exchange = l.at(0);
  _symbol = l.at(1);

  return 0;
}
