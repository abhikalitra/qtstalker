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
#include "DataFactory.h"
#include "SettingFactory.h"

#include <QtDebug>
#include <QStringList>

Data::Data ()
{
  clear();
}

Data::~Data ()
{
  clear();
}

void Data::clear ()
{
  _type = DataFactory::_DATA;
  _delFlag = 1;

  qDeleteAll(_data);
  _data.clear();
}

int Data::type ()
{
  return _type;
}

int Data::dataKeyCount ()
{
  return _data.count();
}

int Data::set (QString k, Setting *d)
{
  Setting *set = _data.value(k);
  if (set)
    delete set;

  _data.insert(k, d);
  return 0;
}

int Data::set (int k, Setting *d)
{
  Setting *set = _data.value(QString::number(k));
  if (set)
    delete set;

  _data.insert(QString::number(k), d);
  return 0;
}

int Data::set (int, Data *)
{
  return 0;
}

Setting * Data::get (QString d)
{
  return _data.value(d);
}

Setting * Data::get (int d)
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

  QHashIterator<QString, Setting *> it(_data);
  while (it.hasNext())
  {
    it.next();
    if (! it.value()->toString().isEmpty())
      l << it.key() + ";" + QString::number(it.value()->type()) + ";" + it.value()->toString();
  }

  return l.join("\n");
}

int Data::fromString (QString d)
{
  _data.clear();

  QStringList l = d.split("\n");

  SettingFactory fac;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList tl = l.at(loop).split(";");
    if (tl.count() != 3)
    {
      qDebug() << "Data::fromString: " << l.at(loop);
      continue;
    }

    Setting *setting = fac.setting(tl.at(1));
    if (! setting)
    {
      qDebug() << "Data::fromString: invalid Setting::Type" << tl.at(1);
      continue;
    }

    if (setting->set(tl.at(2)))
    {
      qDebug() << "Data::fromString: Setting::fromString error" << tl.at(2);
      delete setting;
      continue;
    }

    set(tl.at(0), setting);
  }

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

void Data::setOffset (int)
{
}

int Data::offset ()
{
  return 0;
}

/*
void Data::setCommand (QString d)
{
  _command = d;
}

QString Data::command ()
{
  return _command;
}
*/
/*
void Data::setCommandType (QString d)
{
  _commandType = d;
}

QString Data::commandType ()
{
  return _commandType;
}
*/
/*
void Data::setScriptFile (QString d)
{
  _scriptFile = d;
}

QString Data::scriptFile ()
{
  return _scriptFile;
}
*/

void Data::setDeleteFlag (int d)
{
  _delFlag = d;
}

int Data::deleteFlag ()
{
  return _delFlag;
}
