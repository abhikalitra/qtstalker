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

#include "EAVSearch.h"

#include <QtDebug>

EAVSearch::EAVSearch ()
{
}

int EAVSearch::append (QString key, QString op, QString value)
{
  if (key.isEmpty())
    return 1;

  if (op.isEmpty())
    return 1;

  if (value.isEmpty())
    return 1;

  _keyList << key;
  _opList << op;
  _valueList << value;

  return 0;
}

int EAVSearch::get (int index, QString &key, QString &op, QString &value)
{
  if (index < 0 || index >= _keyList.size())
    return 1;

  key = _keyList.at(index);
  op = _opList.at(index);
  value = _valueList.at(index);

  return 0;
}

int EAVSearch::size ()
{
  return _keyList.size();
}
