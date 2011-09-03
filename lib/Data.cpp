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

#include "Data.h"

#include <QtDebug>
#include <QStringList>

Data::Data ()
{
  clear();
}

void Data::clear ()
{
  _type = "DATA";
  _data.clear();
}

QString Data::type ()
{
  return _type;
}

int Data::dataKeyCount ()
{
  return _data.count();
}

int Data::set (QString k, QVariant d)
{
  _data.insert(k, d);
  return 0;
}

int Data::set (int k, QVariant d)
{
  _data.insert(QString::number(k), d);
  return 0;
}

int Data::set (int, Data *)
{
  return 0;
}

QVariant Data::get (QString d)
{
  return _data.value(d);
}

QVariant Data::get (int d)
{
  return _data.value(QString::number(d));
}

Data * Data::getData (int)
{
  return 0;
}

QString Data::toString ()
{
  QStringList l;

  QHashIterator<QString, QVariant> it(_data);
  while (it.hasNext())
  {
    it.next();
    if (! it.value().toString().isEmpty())
      l << it.key() << it.value().toString();
  }

  return l.join(";");
}

int Data::fromString (QString d)
{
  _data.clear();

  QStringList l = d.split(";");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _data.insert(l.at(loop), QVariant(l.at(loop + 1)));

  return 0;
}

int Data::highLow (double &, double &)
{
  return 0;
}

QList<int> Data::barKeys ()
{
  return QList<int>();
}

QList<QString> Data::dataKeys ()
{
  return _data.keys();
}

int Data::barKeyCount ()
{
  return 0;
}

bool Data::dataContains (QString d)
{
  return _data.contains(d);
}

void Data::barKeyRange (int &, int &)
{
}

void Data::append (Data *)
{
}
