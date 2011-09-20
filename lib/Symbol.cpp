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
#include "DataFactory.h"
#include "SettingFactory.h"

#include <QtDebug>
#include <QStringList>

Symbol::Symbol ()
{
  clear();
}

void Symbol::clear ()
{
  Data::clear();

  _type = DataFactory::_SYMBOL;
  _startIndex = 99999999;
  _endIndex = -99999999;

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

QString Symbol::toString ()
{
  QStringList l;

  QHashIterator<QString, Setting *> it(_data);
  while (it.hasNext())
  {
    it.next();
    l << it.key() + ";" + QString::number(it.value()->type()) + ";" + it.value()->toString();
  }

  QHashIterator<int, Data *> it2(_bars);
  while (it2.hasNext())
  {
    it2.next();
    Data *d = it2.value();

    QList<QString> keys = d->dataKeys();
    int loop = 0;
    for (; loop < keys.count(); loop++)
    {
      Setting *set = d->get(keys.at(loop));
      l << QString::number(it2.key()) + ";" + keys.at(loop) + ";" + QString::number(set->type()) + ";" + set->toString();
    }
  }

  return l.join("\n");
}

int Symbol::fromString (QString d)
{
  clear();

  QStringList l = d.split("\n");

  SettingFactory fac;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList tl = l.at(loop).split(";");

    int k = tl.at(0).toInt();

    switch ((Parm) k)
    {
      case _SYMBOL:
      case _EXCHANGE:
      case _NAME:
      case _TABLE:
      case _TYPE:
      case _LENGTH:
      case _RANGE:
      case _START_DATE:
      case _END_DATE:
      {
        if (tl.count() != 3)
          break;

        Setting *set = fac.setting(tl.at(1));
        if (! set)
          break;

        set->set(tl.at(2));
        _data.insert(tl.at(0), set);
        break;
      }
      default:
      {
        if (tl.count() != 4)
          break;

        Data *b = _bars.value(k);
        if (! b)
        {
          b = new CurveBar;
          set(k, b);
        }

        Setting *set = fac.setting(tl.at(2));
        if (! set)
          break;

        set->set(tl.at(3));
        b->set(tl.at(1).toInt(), set);
        break;
      }
    }
  }

  return 0;
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
