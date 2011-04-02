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

#include "IndicatorSettings.h"
#include "DataDataBase.h"

#include <QHash>

IndicatorSettings::IndicatorSettings ()
{
}

IndicatorSettings::~IndicatorSettings ()
{
  clearAll();
}

void IndicatorSettings::clearAll ()
{
  clear();
  qDeleteAll(_list);
  _list.clear();
}

int IndicatorSettings::load (QString d)
{
  clearAll();
  
  if (d.isEmpty())
    return 1;
  setData("NAME", d);

  Setting set;
  DataDataBase db("indicators");
  if (db.load(d, &set))
  {
    qDebug() << "IndicatorSettings::load: db error";
    return 1;
  }

  QStringList keys;
  set.keyList(keys);

  QHash<int, Setting *> hash;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    QStringList l = keys.at(loop).split(":", QString::SkipEmptyParts);
    if (l.count() == 1)
    {
      setData(keys.at(loop), set.data(keys.at(loop)));
      continue;
    }
    
    Setting *t = hash.value(l.at(0).toInt());
    if (! t)
    {
      t = new Setting;
      hash.insert(l.at(0).toInt(), t);
    }

    t->setData(l.at(1), set.data(keys.at(loop)));
  }

  loop = 0;
  for (; loop < hash.count(); loop++)
    _list.append(hash.value(loop));

  return 0;
}

int IndicatorSettings::save ()
{
  int loop = 0;
  Setting set;
  for (; loop < _list.count(); loop++)
  {
    Setting *t = _list.at(loop);
    QStringList keys;
    t->keyList(keys);

    int loop2 = 0;
    for (; loop2 < keys.count(); loop2++)
      set.setData(QString::number(loop) + ":" + keys.at(loop2), t->data(keys.at(loop2)));
  }

  DataDataBase db("indicators");
  db.transaction();
  
  if (db.removeName(data("NAME")))
  {
    qDebug() << "IndicatorSettings::save: error removing indicator";
    return 1;
  }

  if (db.save(data("NAME"), this))
  {
    qDebug() << "IndicatorSettings::save: db error";
    return 1;
  }

  if (db.save(data("NAME"), &set))
  {
    qDebug() << "IndicatorSettings::save: db error";
    return 1;
  }

  db.commit();
  
  return 0;
}

int IndicatorSettings::count ()
{
  return _list.count();
}

Setting * IndicatorSettings::settings (int d)
{
  if (d < 0 || d >= _list.count())
    return 0;
  
  return _list.at(d);
}

void IndicatorSettings::addSettings (Setting *d)
{
  _list.append(d);
}

void IndicatorSettings::removeSettings (int d)
{
  if (d < 0 || d >= _list.count())
    return;

  Setting *set = _list.at(d);
  if (! set)
    return;
  
  delete set;
  _list.removeAt(d);
}

void IndicatorSettings::copy (IndicatorSettings *d)
{
  Setting::copy(d);

  int loop = 0;
  for (; loop < _list.count(); loop++)
  {
    Setting *set = new Setting;
    _list.at(loop)->copy(set);
    d->addSettings(set);
  }
}
