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

#include <QtDebug>

SettingList::SettingList (int input, int output, QStringList l, QString d)
{
  _type = _LIST;
  _inputType = input;
  _outputType = output;
  _data = l;
  _item = d;
}

SettingList::SettingList (QStringList l, QString d)
{
  _type = _LIST;
  _inputType = _NONE;
  _outputType = _NONE;
  _data = l;
  _item = d;
}

void SettingList::setList (QStringList d)
{
  _data = d;
}

QStringList SettingList::getList ()
{
  return _data;
}

void SettingList::setString (QString d)
{
  _item = d;
}

QString SettingList::getString ()
{
  return _item;
}

int SettingList::fromString (QString d)
{
  _item = d;
  return 0;
}

QString SettingList::toString ()
{
  return _item;
}
