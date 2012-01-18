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
#include "EntityType.h"

#include <QtDebug>

Symbol::Symbol ()
{
  _type = EntityType::_SYMBOL;
  
  Entity::set(QString("TYPE"), Data(QString("Stock")));
  Entity::set(QString("SYMBOL"), Data(QString()));
  Entity::set(QString("NAME"), Data(QString()));
  Entity::set(QString("LENGTH"), Data(-1));
  Entity::set(QString("RANGE"), Data(-1));
  Entity::set(QString("START_DATE"), Data(QDateTime()));
  Entity::set(QString("END_DATE"), Data(QDateTime()));
  Entity::set(QString("TABLE"), Data(QString()));
}
