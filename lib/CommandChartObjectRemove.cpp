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

#include "CommandChartObjectRemove.h"
#include "ThreadMessageFunctions.h"
#include "TypeThreadMessage.h"

#include <QtDebug>

CommandChartObjectRemove::CommandChartObjectRemove ()
{
  _name = "CHART_OBJECT_REMOVE";
  
  Data td(TypeData::_LIST);
  Entity::set(QString("LIST"), td);
}

QString CommandChartObjectRemove::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;

  Data list;
  Entity::toData(QString("LIST"), list);
  
  Entity e;
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_CHART_OBJECT_REMOVE));
  e.set(QString("LIST"), list);
  
  ThreadMessageFunctions tmf;
  tmf.send(e, script);
  
  _returnCode = "OK";
  return _returnCode;
}
