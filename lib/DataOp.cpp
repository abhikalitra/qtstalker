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

#include "DataOp.h"
#include "DataFactory.h"
#include "Operator.h"

#include <QtDebug>

DataOp::DataOp (QString d)
{
  _type = DataFactory::_OP;
  _value = d;

  Operator top;
  _list = top.list();
}

DataOp::DataOp ()
{
  _type = DataFactory::_OP;
  _value = "EQ";

  Operator top;
  _list = top.list();
}

int DataOp::set (QString d)
{
  if (_list.indexOf(d) == -1)
    return 1;

  _value = d;

  return 0;
}

QStringList DataOp::toList ()
{
  return _list;
}

QString DataOp::toString ()
{
  return _value;
}

int DataOp::toInteger ()
{
  return _list.indexOf(_value);
}
