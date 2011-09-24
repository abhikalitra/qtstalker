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
#include "SettingColor.h"
#include "SettingDouble.h"
#include "SettingString.h"
#include "SettingBool.h"
#include "SettingInteger.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "SettingDateTime.h"

#include <QtDebug>

CommandRetracement::CommandRetracement (QObject *p) : Command (p)
{
  _type = "CHART_OBJECT_RETRACEMENT";
}

int CommandRetracement::runScript (Message *sg, Script *script)
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

  // START_DATE
  s = sg->value("START_DATE");
  Setting *date = vdi.setting(SettingFactory::_DATETIME, script, s);
  if (! date)
  {
    _message << "invalid START_DATE " + s;
    return _ERROR;
  }

  // END_DATE
  s = sg->value("END_DATE");
  Setting *date2 = vdi.setting(SettingFactory::_DATETIME, script, s);
  if (! date2)
  {
    _message << "invalid END_DATE " + s;
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Setting *high = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! high)
  {
    _message << "invalid HIGH " + s;
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Setting *low = vdi.setting(SettingFactory::_DOUBLE, script, s);
  if (! low)
  {
    _message << "invalid LOW " + s;
    return _ERROR;
  }

  // LEVEL_1
  Setting *level1 = 0;
  s = sg->value("LEVEL_1");
  if (! s.isEmpty())
  {
    level1 = vdi.setting(SettingFactory::_DOUBLE, script, s);
    if (! level1)
    {
      _message << "invalid LEVEL_1 " + s;
      return _ERROR;
    }
  }

  // LEVEL_2
  Setting *level2 = 0;
  s = sg->value("LEVEL_2");
  if (! s.isEmpty())
  {
    level2 = vdi.setting(SettingFactory::_DOUBLE, script, s);
    if (! level2)
    {
      _message << "invalid LEVEL_2 " + s;
      return _ERROR;
    }
  }

  // LEVEL_3
  Setting *level3 = 0;
  s = sg->value("LEVEL_3");
  if (! s.isEmpty())
  {
    level3 = vdi.setting(SettingFactory::_DOUBLE, script, s);
    if (! level3)
    {
      _message << "invalid LEVEL_3 " + s;
      return _ERROR;
    }
  }

  // LEVEL_4
  Setting *level4 = 0;
  s = sg->value("LEVEL_4");
  if (! s.isEmpty())
  {
    level4 = vdi.setting(SettingFactory::_DOUBLE, script, s);
    if (! level4)
    {
      _message << "invalid LEVEL_4 " + s;
      return _ERROR;
    }
  }

  // LEVEL_5
  Setting *level5 = 0;
  s = sg->value("LEVEL_5");
  if (! s.isEmpty())
  {
    level5 = vdi.setting(SettingFactory::_DOUBLE, script, s);
    if (! level5)
    {
      _message << "invalid LEVEL_5 " + s;
      return _ERROR;
    }
  }

  // LEVEL_6
  Setting *level6 = 0;
  s = sg->value("LEVEL_6");
  if (! s.isEmpty())
  {
    level6 = vdi.setting(SettingFactory::_DOUBLE, script, s);
    if (! level6)
    {
      _message << "invalid LEVEL_6 " + s;
      return _ERROR;
    }
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
  co->set(ChartObjectData::_DATE_2, new SettingDateTime(date2->toDateTime()));
  co->set(ChartObjectData::_HIGH, new SettingDouble(high->toDouble()));
  co->set(ChartObjectData::_LOW, new SettingDouble(low->toDouble()));
  co->set(ChartObjectData::_CHART, new SettingString(chart->toString()));
  co->set(ChartObjectData::_Z, new SettingInteger(z->toInteger()));
  co->set(ChartObjectData::_ID, new SettingInteger(id));
  co->set(ChartObjectData::_RO, new SettingBool(TRUE));
  co->set(ChartObjectData::_TYPE, new SettingString(QString("Retracement")));

  if (level1)
    co->set(ChartObjectData::_LEVEL_1, new SettingDouble(level1->toDouble()));
  if (level2)
    co->set(ChartObjectData::_LEVEL_2, new SettingDouble(level2->toDouble()));
  if (level3)
    co->set(ChartObjectData::_LEVEL_3, new SettingDouble(level3->toDouble()));
  if (level4)
    co->set(ChartObjectData::_LEVEL_4, new SettingDouble(level4->toDouble()));
  if (level5)
    co->set(ChartObjectData::_LEVEL_5, new SettingDouble(level5->toDouble()));
  if (level6)
    co->set(ChartObjectData::_LEVEL_6, new SettingDouble(level6->toDouble()));

  script->setData(QString::number(id), co);

  return _OK;
}
