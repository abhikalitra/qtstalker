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

#include "DataSymbol.h"
#include "DataFactory.h"

#include <QtDebug>

DataSymbol::DataSymbol (QStringList d)
{
  _type = DataFactory::_SYMBOL;
  _value = d;
}

DataSymbol::DataSymbol ()
{
  _type = DataFactory::_SYMBOL;
}

int DataSymbol::set (QStringList d)
{
  _value = d;
  return 0;
}

int DataSymbol::set (QString d)
{
  _value = d.split(",", QString::SkipEmptyParts);

  return 0;
}

QString DataSymbol::toString ()
{
  return _value.join(",");
}

QStringList DataSymbol::toList ()
{
  return _value;
}
