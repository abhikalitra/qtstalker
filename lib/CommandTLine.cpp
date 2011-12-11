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
#include "ChartObjectData.h"
#include "DataColor.h"
#include "DataDouble.h"
#include "DataString.h"
#include "DataBool.h"
#include "DataInteger.h"
#include "DataDateTime.h"

#include <QtDebug>

CommandTLine::CommandTLine (QObject *p) : Command (p)
{
  _name = "CHART_OBJECT_TLINE";

  _values.insert(_ParmTypeChart, new DataString());
  _values.insert(_ParmTypeColor, new DataColor(QColor(Qt::green)));
  _values.insert(_ParmTypeDateStart, new DataDateTime(QDateTime::currentDateTime()));
  _values.insert(_ParmTypeDateEnd, new DataDateTime(QDateTime::currentDateTime()));
  _values.insert(_ParmTypePriceStart, new DataDouble(0));
  _values.insert(_ParmTypePriceEnd, new DataDouble(0));
  _values.insert(_ParmTypeZ, new DataInteger(0));
  _values.insert(_ParmTypePen, new DataInteger(1));
}

void CommandTLine::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandText::runScript: parse error");
    return;
  }

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_COLOR, new DataColor(_values.value(_ParmTypeColor)->toColor()));
  co->set(ChartObjectData::_DATE, new DataDateTime(_values.value(_ParmTypeDateStart)->toDateTime()));
  co->set(ChartObjectData::_DATE_2, new DataDateTime(_values.value(_ParmTypeDateEnd)->toDateTime()));
  co->set(ChartObjectData::_PRICE, new DataDouble(_values.value(_ParmTypePriceStart)->toDouble()));
  co->set(ChartObjectData::_PRICE_2, new DataDouble(_values.value(_ParmTypePriceEnd)->toDouble()));
  co->set(ChartObjectData::_CHART, new DataString(_values.value(_ParmTypeChart)->toString()));
  co->set(ChartObjectData::_Z, new DataInteger(_values.value(_ParmTypeZ)->toInteger()));
  co->set(ChartObjectData::_PEN, new DataInteger(_values.value(_ParmTypePen)->toInteger()));
  co->set(ChartObjectData::_ID, new DataInteger(id));
  co->set(ChartObjectData::_RO, new DataBool(TRUE));
  co->set(ChartObjectData::_TYPE, new DataString(QString("TLine")));

  script->setData(QString::number(id), co);

  Command::done(QString());
}
