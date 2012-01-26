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

#include "CommandTLine.h"
#include "ChartObjectTLine.h"
#include "TypeEntity.h"
#include "KeyChartObject.h"
#include "Script.h"

#include <QtDebug>

CommandTLine::CommandTLine ()
{
  _name = "CHART_OBJECT_TLINE";

  ChartObjectTLine co;
  Entity settings = co.settings();
  
  Data td;
  settings.set(QString("CHART"), td);
  
  QHash<QString, Data> tdata;
  settings.data(tdata);
  Entity::set(tdata);
}

QString CommandTLine::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;

  Entity co;
  QHash<QString, Data> tdata;
  Entity::data(tdata);
  co.set(tdata);
  co.setType(TypeEntity::_CHART_OBJECT);
  
  KeyChartObject keys;
  co.set(keys.indexToString(KeyChartObject::_RO), Data(TRUE));

  // update with new ID
  ChartObjectTLine tline;
  QString s;
  tline.newID(s);
  co.setName(s);
  co.set(keys.indexToString(KeyChartObject::_ID), Data(s));

  script->setData(co.name(), co);

  _returnCode = "OK";
  return _returnCode;
}
