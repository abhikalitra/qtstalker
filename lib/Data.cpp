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

int Data::type ()
{
  return _type;
}

void Data::setDeleteFlag (int d)
{
  _delFlag = d;
}

int Data::deleteFlag ()
{
  return _delFlag;
}

void Data::setTab (int d)
{
  _tab = d;
}

int Data::tab ()
{
  return _tab;
}

void Data::setLabel (QString d)
{
  _label = d;
}

QString Data::label ()
{
  return _label;
}

// virtual functions

void Data::clear ()
{
  _delFlag = 1;
  _tab = 0;
}

int Data::set (int, Data *)
{
  return 0;
}

int Data::set (QString)
{
  return 0;
}

int Data::set (QStringList)
{
  return 0;
}

int Data::set (int)
{
  return 0;
}

int Data::set (double)
{
  return 0;
}

int Data::set (bool)
{
  return 0;
}

int Data::set (QColor)
{
  return 0;
}

int Data::set (QFont)
{
  return 0;
}

int Data::set (QDateTime)
{
  return 0;
}

QString Data::toString ()
{
  return QString();
}

QStringList Data::toList ()
{
  return QStringList();
}

int Data::toInteger ()
{
  return 0;
}

double Data::toDouble ()
{
  return 0;
}

bool Data::toBool ()
{
  return FALSE;
}

QColor Data::toColor ()
{
  return QColor();
}

QFont Data::toFont ()
{
  return QFont();
}

QDateTime Data::toDateTime ()
{
  return QDateTime();
}

Data * Data::toData (int)
{
  return 0;
}

int Data::highLow (double &, double &)
{
  return 0;
}

QList<int> Data::keys ()
{
  return QList<int>();
}

int Data::keyCount ()
{
  return 0;
}

void Data::keyRange (int &, int &)
{
}

void Data::append (Data *)
{
}

QString Data::toSaveString ()
{
  return QString();
}

int Data::fromSaveString (QString)
{
  return 0;
}

/*
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
*/
