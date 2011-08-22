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

#include "SettingGroup.h"
#include "SettingString.h"

#include <QtDebug>

SettingGroup::SettingGroup ()
{
}

SettingGroup::~SettingGroup ()
{
  clear();
}

void SettingGroup::clear ()
{
  qDeleteAll(_settings);
  _settings.clear();

  _keys.clear();
}

void SettingGroup::set (Setting *d)
{
  Setting *tset = _settings.value(d->key());
  if (tset)
    delete tset;

  _settings.insert(d->key(), d);

  if (! _keys.contains(d->key()))
    _keys << d->key();
}

Setting * SettingGroup::get (QString key)
{
  return _settings.value(key);
}

QStringList SettingGroup::keys ()
{
  return _keys;
}

bool SettingGroup::contains (QString k)
{
  return _settings.contains(k);
}

void SettingGroup::merge (SettingGroup *d)
{
  QStringList keys = d->keys();
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Setting *set = get(keys.at(loop));
    if (! set)
      continue;

    Setting *set2 = d->get(keys.at(loop));
    set->fromString(set2->toString());
  }
}

QString SettingGroup::toString ()
{
  QStringList l;
  QHashIterator<QString, Setting *> it(_settings);
  while (it.hasNext())
  {
    it.next();
    l << it.key() + "=" + it.value()->toString();
  }

  return l.join(",");
}

int SettingGroup::fromString (QString d)
{
  clear();

  QStringList l = d.split(",");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split("=");
    if (l2.count() != 2)
      return 1;

    SettingString *ss = new SettingString(l2.at(1));
    ss->setKey(l2.at(0));
    set(ss);
  }

  return 0;
}

void SettingGroup::setCommand (QString d)
{
  _command = d;
}

QString SettingGroup::command ()
{
  return _command;
}

void SettingGroup::setStepName (QString d)
{
  _stepName = d;
}

QString SettingGroup::stepName ()
{
  return _stepName;
}

int SettingGroup::parse (QStringList &l)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString s = l.at(loop).trimmed();
    QStringList tl = s.split("=");
    if (tl.count() != 2)
      return 1;

    tl.at(0).trimmed();
    tl.at(1).trimmed();

    if (tl.at(0) == "STEP")
    {
      _stepName = tl.at(1);
      continue;
    }

    if (tl.at(0) == "COMMAND")
    {
      _command = tl.at(1);
      continue;
    }

    SettingString *ss = new SettingString(tl.at(1));
    ss->setKey(tl.at(0));
    set(ss);
  }

  return 0;
}
