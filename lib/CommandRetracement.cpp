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

#include "CommandRetracement.h"
#include "ChartObjectData.h"
#include "DataColor.h"
#include "DataDouble.h"
#include "DataString.h"
#include "DataBool.h"
#include "DataInteger.h"
#include "DataDateTime.h"

#include <QtDebug>

CommandRetracement::CommandRetracement (QObject *p) : Command (p)
{
  _name = "CHART_OBJECT_RETRACEMENT";

  _values.insert(_ParmTypeChart, new DataString);
  _values.insert(_ParmTypeColor, new DataColor(QColor(Qt::red)));
  _values.insert(_ParmTypeDateStart, new DataDateTime(QDateTime::currentDateTime()));
  _values.insert(_ParmTypeDateEnd, new DataDateTime(QDateTime::currentDateTime()));
  _values.insert(_ParmTypeHigh, new DataDouble(0));
  _values.insert(_ParmTypeLow, new DataDouble(0));
  _values.insert(_ParmTypeZ, new DataInteger(0));
  _values.insert(_ParmTypePen, new DataInteger(1));
  _values.insert(_ParmTypeLevel1, new DataDouble(23.6));
  _values.insert(_ParmTypeLevel2, new DataDouble(38.2));
  _values.insert(_ParmTypeLevel3, new DataDouble(50));
  _values.insert(_ParmTypeLevel4, new DataDouble(61.8));
  _values.insert(_ParmTypeLevel5, new DataDouble(78.6));
}

void CommandRetracement::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandRetracement::runScript: parse error");
    return;
  }

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_COLOR, new DataColor(_values.value(_ParmTypeColor)->toColor()));
  co->set(ChartObjectData::_DATE, new DataDateTime(_values.value(_ParmTypeDateStart)->toDateTime()));
  co->set(ChartObjectData::_DATE_2, new DataDateTime(_values.value(_ParmTypeDateEnd)->toDateTime()));
  co->set(ChartObjectData::_HIGH, new DataDouble(_values.value(_ParmTypeHigh)->toDouble()));
  co->set(ChartObjectData::_LOW, new DataDouble(_values.value(_ParmTypeLow)->toDouble()));
  co->set(ChartObjectData::_CHART, new DataString(_values.value(_ParmTypeChart)->toString()));
  co->set(ChartObjectData::_Z, new DataInteger(_values.value(_ParmTypeZ)->toInteger()));
  co->set(ChartObjectData::_PEN, new DataInteger(_values.value(_ParmTypePen)->toInteger()));
  co->set(ChartObjectData::_ID, new DataInteger(id));
  co->set(ChartObjectData::_RO, new DataBool(TRUE));
  co->set(ChartObjectData::_TYPE, new DataString(QString("Retracement")));

  co->set(ChartObjectData::_LEVEL_1, new DataDouble(_values.value(_ParmTypeLevel1)->toDouble()));
  co->set(ChartObjectData::_LEVEL_2, new DataDouble(_values.value(_ParmTypeLevel2)->toDouble()));
  co->set(ChartObjectData::_LEVEL_3, new DataDouble(_values.value(_ParmTypeLevel3)->toDouble()));
  co->set(ChartObjectData::_LEVEL_4, new DataDouble(_values.value(_ParmTypeLevel4)->toDouble()));
  co->set(ChartObjectData::_LEVEL_5, new DataDouble(_values.value(_ParmTypeLevel5)->toDouble()));

  script->setData(QString::number(id), co);

  Command::done(QString());
}
