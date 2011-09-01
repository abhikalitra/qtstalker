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

int Data::set (QString k, QString d)
{
  _data.insert(k, d);
  return 0;
}

int Data::set (int k, QString d)
{
  _data.insert(QString::number(k), d);
  return 0;
}

int Data::set (int k, QDateTime d)
{
  _data.insert(QString::number(k), d.toString(Qt::ISODate));
  return 0;
}

int Data::set (QString k, QDateTime d)
{
  _data.insert(k, d.toString(Qt::ISODate));
  return 0;
}

int Data::set (int k, QColor d)
{
  _data.insert(QString::number(k), d.name());
  return 0;
}

int Data::set (QString k, QColor d)
{
  _data.insert(k, d.name());
  return 0;
}

int Data::set (int k, double d)
{
  _data.insert(QString::number(k), QString::number(d));
  return 0;
}

int Data::set (QString k, double d)
{
  _data.insert(k, QString::number(d));
  return 0;
}

int Data::set (int k, int d)
{
  _data.insert(QString::number(k), QString::number(d));
  return 0;
}

int Data::set (QString k, int d)
{
  _data.insert(k, QString::number(d));
  return 0;
}

int Data::set (int k, QStringList d)
{
  _data.insert(QString::number(k), d.join(";"));
  return 0;
}

int Data::set (QString k, QStringList d)
{
  _data.insert(k, d.join(";"));
  return 0;
}

int Data::set (int k, bool d)
{
  _data.insert(QString::number(k), QString::number(d));
  return 0;
}

int Data::set (QString k, bool d)
{
  _data.insert(k, QString::number(d));
  return 0;
}

int Data::set (int, Data *)
{
  return 0;
}

int Data::set (QString, Data *)
{
  return 0;
}

int Data::set (int k, QFont d)
{
  _data.insert(QString::number(k), d.toString());
  return 0;
}

int Data::set (QString k, QFont d)
{
  _data.insert(k, d.toString());
  return 0;
}

QString Data::get (QString d)
{
  return _data.value(d);
}

QString Data::get (int d)
{
  return _data.value(QString::number(d));
}

QDateTime Data::getDateTime (int d)
{
  return QDateTime::fromString(_data.value(QString::number(d)), Qt::ISODate);
}

QColor Data::getColor (int d)
{
  return QColor(_data.value(QString::number(d)));
}

QColor Data::getColor (QString d)
{
  return QColor(_data.value(d));
}

double Data::getDouble (int d)
{
  return _data.value(QString::number(d)).toDouble();
}

double Data::getDouble (QString d)
{
  return _data.value(d).toDouble();
}

int Data::getInteger (int d)
{
  return _data.value(QString::number(d)).toInt();
}

int Data::getInteger (QString d)
{
  return _data.value(d).toInt();
}

QStringList Data::getList (int d)
{
  return _data.value(QString::number(d)).split(";");
}

bool Data::getBool (int d)
{
  return (bool) _data.value(QString::number(d)).toInt();
}

bool Data::getBool (QString d)
{
  return (bool) _data.value(d).toInt();
}

Data * Data::getData (int)
{
  return 0;
}

QFont Data::getFont (int d)
{
  QFont f;
  f.fromString(_data.value(QString::number(d)));
  return f;
}

QString Data::toString ()
{
  QStringList l;

  QHashIterator<QString, QString> it(_data);
  while (it.hasNext())
  {
    it.next();
    if (! it.value().isEmpty())
      l << it.key() << it.value();
  }

  return l.join(";");
}

int Data::fromString (QString d)
{
  _data.clear();

  QStringList l = d.split(";");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _data.insert(l.at(loop), l.at(loop + 1));

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
