/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "Operator.h"

Operator::Operator ()
{
  _list << "<" << "<=" << "=" << "!=" << "=>" << ">";
}

QStringList & Operator::list ()
{
  return _list;
}

Operator::Type Operator::stringToOperator (QString d)
{
  return (Operator::Type) _list.indexOf(d);
}

int Operator::test (double val, Operator::Type op, double val2)
{
  int rc = 0;

  switch (op)
  {
    case _LESS_THAN:
      if (val < val2)
        rc = 1;
      break;
    case _LESS_THAN_EQUAL:
      if (val <= val2)
        rc = 1;
      break;
    case _EQUAL:
      if (val == val2)
        rc = 1;
      break;
    case _NOT_EQUAL:
      if (val != val2)
        rc = 1;
      break;
    case _GREATER_THAN_EQUAL:
      if (val >= val2)
        rc = 1;
      break;
    case _GREATER_THAN:
      if (val > val2)
        rc = 1;
      break;
    default:
      break;
  }

  return rc;
}
