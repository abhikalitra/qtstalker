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

#include "DataMA.h"
#include "DataFactory.h"
#include "MAType.h"

#include <QtDebug>

DataMA::DataMA (QString d)
{
  _type = DataFactory::_MA;
  _value = d;

  MAType mat;
  _list = mat.list();
}

DataMA::DataMA ()
{
  _type = DataFactory::_MA;

  MAType mat;
  _list = mat.list();
}

int DataMA::set (QString d)
{
  if (_list.indexOf(d) == -1)
    return 1;

  _value = d;

  return 0;
}

QStringList DataMA::toList ()
{
  return _list;
}

QString DataMA::toString ()
{
  return _value;
}

int DataMA::toInteger ()
{
  return _list.indexOf(_value);
}
