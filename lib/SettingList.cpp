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

#include "SettingList.h"
#include "SettingFactory.h"
#include "DataFactory.h"
#include "DataSetting.h"
#include "Script.h"

#include <QtDebug>

SettingList::SettingList (QString d)
{
  _type = SettingFactory::_LIST;
  _value = d;
}

SettingList::SettingList ()
{
  _type = SettingFactory::_LIST;
}

int SettingList::set (QString d)
{
  _value = d;

  return 0;
}

QStringList SettingList::toList ()
{
  return _list;
}

QString SettingList::toString ()
{
  return _value;
}

void SettingList::set (QStringList d)
{
  _list = d;
}

int SettingList::set (QString d, void *d2)
{
  Script *script = (Script *) d2;

  Data *td = script->data(d);
  if (! td)
    return 1;

  if (td->type() != DataFactory::_DATA_SETTING)
    return 1;

  Setting *set = td->get(DataSetting::_VALUE);
  if (! set)
    return 1;

  if (set->type() != SettingFactory::_LIST)
    return 1;

  _value = set->toString();
  _list = set->toList();

  return 0;
}
