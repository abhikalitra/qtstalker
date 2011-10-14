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

#include "DataList.h"
#include "DataFactory.h"

#include <QtDebug>

DataList::DataList (QString d)
{
  _type = DataFactory::_LIST;
  _value = d;
}

DataList::DataList ()
{
  _type = DataFactory::_LIST;
}

int DataList::set (QString d)
{
  if (_list.indexOf(d) == -1)
    return 1;

  _value = d;

  return 0;
}

QStringList DataList::toList ()
{
  return _list;
}

QString DataList::toString ()
{
  return _value;
}

int DataList::set (QStringList d)
{
  _list = d;
  return 0;
}

int DataList::toInteger ()
{
  return _list.indexOf(_value);
}
