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
#include "ChartObjectData.h"
#include "SettingColor.h"
#include "SettingString.h"
#include "SettingBool.h"
#include "SettingInteger.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "SettingDateTime.h"

#include <QtDebug>

CommandVLine::CommandVLine (QObject *p) : Command (p)
{
  _type = "CHART_OBJECT_VLINE";
}

int CommandVLine::runScript (Message *sg, Script *script)
{
  // color
  VerifyDataInput vdi;
  QString s = sg->value("COLOR");
  Setting *color = vdi.setting(SettingFactory::_COLOR, script, s);
  if (! color)
  {
    _message << "invalid COLOR " + s;
    return _ERROR;
  }

  // DATE
  s = sg->value("DATE");
  Setting *date = vdi.setting(SettingFactory::_DATETIME, script, s);
  if (! date)
  {
    _message << "invalid DATETIME " + s;
    return _ERROR;
  }

  // CHART
  s = sg->value("CHART");
  Setting *chart = vdi.setting(SettingFactory::_STRING, script, s);
  if (! chart)
  {
    _message << "invalid CHART " + s;
    return _ERROR;
  }

  // Z
  s = sg->value("Z");
  Setting *z = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! z)
  {
    _message << "invalid Z " + s;
    return _ERROR;
  }

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_COLOR, new SettingColor(color->toColor()));
  co->set(ChartObjectData::_DATE, new SettingDateTime(date->toDateTime()));
  co->set(ChartObjectData::_CHART, new SettingString(chart->toString()));
  co->set(ChartObjectData::_Z, new SettingInteger(z->toInteger()));
  co->set(ChartObjectData::_ID, new SettingInteger(id));
  co->set(ChartObjectData::_RO, new SettingBool(TRUE));
  co->set(ChartObjectData::_TYPE, new SettingString(QString("VLine")));

  script->setData(QString::number(id), co);

  return _OK;
}
