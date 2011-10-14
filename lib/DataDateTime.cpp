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

#include "DataDateTime.h"
#include "DataFactory.h"

#include <QtDebug>

DataDateTime::DataDateTime (QDateTime d)
{
  _type = DataFactory::_DATETIME;
  _value = d;
}

DataDateTime::DataDateTime ()
{
  _type = DataFactory::_DATETIME;
  _value = QDateTime::currentDateTime();
}

int DataDateTime::set (QString d)
{
  QDateTime dt = QDateTime::fromString(d, Qt::ISODate);
  if (! dt.isValid())
    return 1;

  _value = dt;

  return 0;
}

QString DataDateTime::toString ()
{
  return _value.toString(Qt::ISODate);
}

QDateTime DataDateTime::toDateTime ()
{
  return _value;
}

int DataDateTime::set (QDateTime d)
{
  _value = d;
  return 0;
}
