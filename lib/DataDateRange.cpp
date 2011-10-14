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

#include "DataDateRange.h"
#include "DataFactory.h"
#include "DateRange.h"

#include <QtDebug>

DataDateRange::DataDateRange (QString d)
{
  _type = DataFactory::_DATE_RANGE;
  _value = d;

  DateRange dr;
  _list = dr.list();
}

DataDateRange::DataDateRange ()
{
  _type = DataFactory::_DATE_RANGE;

  DateRange dr;
  _list = dr.list();
}

int DataDateRange::set (QString d)
{
  if (_list.indexOf(d) == -1)
    return 1;

  _value = d;

  return 0;
}

QStringList DataDateRange::toList ()
{
  return _list;
}

QString DataDateRange::toString ()
{
  return _value;
}

int DataDateRange::toInteger ()
{
  return _list.indexOf(_value);
}
