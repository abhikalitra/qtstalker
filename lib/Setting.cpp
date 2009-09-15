/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "Setting.h"
#include <QObject>

Setting::Setting ()
{
}

void Setting::getData (QString &k, QString &d)
{
  d.clear();
  if (dict.contains(k))
    d = dict.value(k);
}

double Setting::getDouble (QString &k)
{
  if (dict.contains(k))
    return dict.value(k).toDouble();
  else
    return 0;
}

int Setting::getInt (QString &k)
{
  if (dict.contains(k))
    return dict.value(k).toInt();
  else
    return 0;
}

void Setting::setData (QString &k, QString &d)
{
  dict.insert(k, d);
}

void Setting::getKeyList (QStringList &l)
{
  l.clear();
  QHashIterator<QString, QString> it(dict);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }
}

void Setting::remove (QString &k)
{
  dict.remove(k);
}

void Setting::getString (QString &s)
{
  s.clear();
  QStringList l;
  QHashIterator<QString, QString> it(dict);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key() + "=" + it.value());
  }
  s = l.join("|");
}

void Setting::parse (QString &d)
{
  dict.clear();

  QStringList l = d.split("|");

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QStringList l2 = l[loop].split("=");
    if (l2.count() > 1)
      dict.insert(l2[0], l2[1]);
  }
}

void Setting::clear ()
{
  dict.clear();
}

int Setting::count ()
{
  return (int) dict.count();
}

void Setting::copy (Setting *r)
{
  QString k, d;
  QHashIterator<QString, QString> it(dict);
  while (it.hasNext())
  {
    it.next();
    k = it.key();
    d = it.value();
    r->setData(k, d);
  }
}

