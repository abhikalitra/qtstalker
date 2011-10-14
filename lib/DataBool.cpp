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

#include "DataBool.h"
#include "DataFactory.h"

#include <QtDebug>

DataBool::DataBool (bool d)
{
  _type = DataFactory::_BOOL;
  _value = d;
}

DataBool::DataBool ()
{
  _type = DataFactory::_BOOL;
  _value = FALSE;
}

int DataBool::set (QString d)
{
  if (d == "0" || d == "false" || d == "FALSE")
    _value = FALSE;
  else
  {
    if (d == "1" || d == "true" || d == "TRUE")
      _value = TRUE;
    else
      return 1;
  }

  return 0;
}

QString DataBool::toString ()
{
  return QString::number(_value);
}

bool DataBool::toBool ()
{
  return _value;
}

int DataBool::set (bool d)
{
  _value = d;
  return 0;
}
