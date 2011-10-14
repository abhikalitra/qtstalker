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

#include "DataFont.h"
#include "DataFactory.h"

#include <QtDebug>

DataFont::DataFont (QFont d)
{
  _type = DataFactory::_FONT;
  _value = d;
}

DataFont::DataFont ()
{
  _type = DataFactory::_FONT;
  _value = QFont();
}

int DataFont::set (QString d)
{
  QFont f;
  if (! f.fromString(d))
    return 1;

  _value = f;

  return 0;
}

QString DataFont::toString ()
{
  return _value.toString();
}

QFont DataFont::toFont ()
{
  return _value;
}

int DataFont::set (QFont d)
{
  _value = d;
  return 0;
}
