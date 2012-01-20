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

#include "CommandVLine.h"
#include "ChartObjectVLine.h"
#include "TypeEntity.h"
#include "KeyChartObject.h"
#include "Script.h"

#include <QtDebug>

CommandVLine::CommandVLine ()
{
  _name = "CHART_OBJECT_VLINE";

  ChartObjectVLine co;
  Entity settings = co.settings();
  
  Data td;
  settings.set(QString("CHART"), td);
  
  Entity::set(settings.data());
}

QString CommandVLine::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;

  Entity co;
  co.set(Entity::data());
  co.setType(TypeEntity::_CHART_OBJECT);
  
  KeyChartObject keys;
  co.set(keys.indexToString(KeyChartObject::_RO), Data(TRUE));

  // update with new ID
  ChartObjectVLine vline;
  QString s;
  vline.newID(s);
  co.setName(s);
  co.set(keys.indexToString(KeyChartObject::_ID), Data(s));

  script->setData(co.name(), co);

  _returnCode = "OK";
  return _returnCode;
}
