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

#include "Symbol.h"
#include "TypeEntity.h"
#include "KeySymbol.h"

#include <QtDebug>

Symbol::Symbol ()
{
  _type = TypeEntity::_SYMBOL;
  
  KeySymbol keys;
  Entity::set(keys.indexToString(KeySymbol::_TYPE), Data(QString("Stock")));
  Entity::set(keys.indexToString(KeySymbol::_SYMBOL), Data(QString()));
  Entity::set(keys.indexToString(KeySymbol::_NAME), Data(QString()));
  Entity::set(keys.indexToString(KeySymbol::_LENGTH), Data(-1));
  Entity::set(keys.indexToString(KeySymbol::_RANGE), Data(-1));
  Entity::set(keys.indexToString(KeySymbol::_START_DATE), Data(QDateTime()));
  Entity::set(keys.indexToString(KeySymbol::_END_DATE), Data(QDateTime()));
  Entity::set(keys.indexToString(KeySymbol::_TABLE), Data(QString()));
}
