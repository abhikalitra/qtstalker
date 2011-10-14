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

#include "DataColor.h"
#include "DataFactory.h"

#include <QtDebug>

DataColor::DataColor (QColor d)
{
  _type = DataFactory::_COLOR;
  _value = d;
}

DataColor::DataColor ()
{
  _type = DataFactory::_COLOR;
  _value = QColor("red");
}

int DataColor::set (QString d)
{
  QColor c(d);
  if (! c.isValid())
    return 1;

  _value = c;

  return 0;
}

QString DataColor::toString ()
{
  return _value.name();
}

QColor DataColor::toColor ()
{
  return _value;
}

int DataColor::set (QColor d)
{
  _value = d;
  return 0;
}
