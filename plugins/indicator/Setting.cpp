/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
#include <qobject.h>

Setting::Setting ()
{
  dict.setAutoDelete(TRUE);
}

Setting::~Setting ()
{
}

void Setting::set (QString k, QString d, Setting::Type t)
{
  SettingItem *set = new SettingItem;
  set->key = k;
  set->data = d;
  set->type = t;

  switch (t)
  {
    case LineType:
      set->list.append(QObject::tr("Dot"));
      set->list.append(QObject::tr("Dash"));
      set->list.append(QObject::tr("Histogram"));
      set->list.append(QObject::tr("Histogram Bar"));
      set->list.append(QObject::tr("Line"));
      break;
    case MAType:
      set->list.append(QObject::tr("EMA"));
      set->list.append(QObject::tr("SMA"));
      set->list.append(QObject::tr("WMA"));
      break;
    case InputField:
      set->list.append(QObject::tr("Open"));
      set->list.append(QObject::tr("High"));
      set->list.append(QObject::tr("Low"));
      set->list.append(QObject::tr("Close"));
      set->list.append(QObject::tr("Volume"));
      set->list.append(QObject::tr("Open Interest"));
      break;
    default:
      break;
  }

  dict.replace(k, set);
}

QString Setting::getData (QString k)
{
  SettingItem *set = dict[k];
  if (set)
    return set->data;
  else
    return 0;
}

double Setting::getFloat (QString k)
{
  SettingItem *set = dict[k];
  if (set)
    return set->data.toDouble();
  else
    return 0;
}

int Setting::getInt (QString k)
{
  SettingItem *set = dict[k];
  if (set)
    return set->data.toInt();
  else
    return 0;
}

void Setting::setData (QString k, QString d)
{
  SettingItem *set = dict[k];
  if (set)
    set->data = d;
}

Setting::Type Setting::getType (QString k)
{
  SettingItem *set = dict[k];
  if (set)
    return (Setting::Type) set->type;
  else
    return Setting::None;
}

QStringList Setting::getList (QString k)
{
  SettingItem *set = dict[k];
  if (set)
    return set->list;
  else
    return 0;
}

void Setting::setList (QString k, QStringList l)
{
  SettingItem *set = dict[k];
  if (set)
    set->list = l;
}

QStringList Setting::getKeyList ()
{
  QStringList l;

  QDictIterator<SettingItem> it(dict);
  while (it.current())
  {
    l.append(it.currentKey());
    ++it;
  }

  return l;
}

QStringList Setting::getStringList ()
{
  QStringList l;
  QDictIterator<SettingItem> it(dict);
  while (it.current())
  {
    SettingItem *item = it.current();
    l.append(item->key);
    l.append(item->data);
    l.append(QString::number(item->type));
    ++it;
  }

  return l;
}

void Setting::remove (QString k)
{
  dict.remove(k);
}

QString Setting::getString ()
{
  QStringList l = getStringList();
  return l.join(",");
}

void Setting::parse (QString d)
{
  dict.clear();

  QStringList l(QStringList::split(",", d, FALSE));

  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 3)
    set(l[loop], l[loop + 1], (Setting::Type) l[loop + 2].toInt());
}

void Setting::parse (QStringList d)
{
  dict.clear();

  int loop;
  for (loop = 0; loop < (int) d.count(); loop = loop + 3)
    set(d[loop], d[loop + 1], (Setting::Type) d[loop + 2].toInt());
}

SettingItem * Setting::getItem (QString k)
{
  SettingItem *set = dict[k];
  if (set)
    return set;
  else
    return 0;
}

void Setting::clear ()
{
  dict.clear();
}

int Setting::count ()
{
  return (int) dict.count();
}


