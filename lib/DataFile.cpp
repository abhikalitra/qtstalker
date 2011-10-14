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

#include "DataFile.h"
#include "DataFactory.h"

#include <QtDebug>

DataFile::DataFile (QStringList d)
{
  _type = DataFactory::_FILE;
  _value = d;
}

DataFile::DataFile ()
{
  _type = DataFactory::_FILE;
}

int DataFile::set (QStringList d)
{
  _value = d;
  return 0;
}

int DataFile::set (QString d)
{
  _value = d.split(",");

  return 0;
}

QString DataFile::toString ()
{
  return _value.join(",");
}

QStringList DataFile::toList ()
{
  return _value;
}
