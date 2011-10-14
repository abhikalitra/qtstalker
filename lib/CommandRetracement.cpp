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
#include "VerifyDataInput.h"
#include "DataDateTime.h"

#include <QtDebug>

CommandRetracement::CommandRetracement (QObject *p) : Command (p)
{
  _name = "CHART_OBJECT_RETRACEMENT";
}

int CommandRetracement::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // color
  QColor color;
  QString s = sg->value("COLOR");
  if (vdi.toColor(script, s, color))
  {
    qDebug() << "CommandRetracement::runScript: invalid COLOR, using default" << s;
    color = QColor(Qt::red);
  }

  // START_DATE
  QDateTime date;
  s = sg->value("START_DATE");
  if (vdi.toDateTime(script, s, date))
  {
    qDebug() << "CommandRetracement::runScript: invalid START_DATE, using default" << s;
    date = QDateTime::currentDateTime();
  }

  // END_DATE
  QDateTime date2;
  s = sg->value("END_DATE");
  if (vdi.toDateTime(script, s, date2))
  {
    qDebug() << "CommandRetracement::runScript: invalid END_DATE, using default" << s;
    date2 = QDateTime::currentDateTime();
  }

  // HIGH
  double high = 0;
  s = sg->value("HIGH");
  if (vdi.toDouble(script, s, high))
  {
    qDebug() << "CommandRetracement::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LOW
  double low = 0;
  s = sg->value("LOW");
  if (vdi.toDouble(script, s, low))
  {
    qDebug() << "CommandRetracement::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LEVEL_1
  double level1 = 0;
  s = sg->value("LEVEL_1");
  if (vdi.toDouble(script, s, level1))
  {
    qDebug() << "CommandRetracement::runScript: invalid LEVEL_1, using default" << s;
    level1 = 0;
  }

  // LEVEL_2
  double level2 = 0;
  s = sg->value("LEVEL_2");
  if (vdi.toDouble(script, s, level2))
  {
    qDebug() << "CommandRetracement::runScript: invalid LEVEL_2, using default" << s;
    level2 = 0;
  }

  // LEVEL_3
  double level3 = 0;
  s = sg->value("LEVEL_3");
  if (vdi.toDouble(script, s, level3))
  {
    qDebug() << "CommandRetracement::runScript: invalid LEVEL_3, using default" << s;
    level3 = 0;
  }

  // LEVEL_4
  double level4 = 0;
  s = sg->value("LEVEL_4");
  if (vdi.toDouble(script, s, level4))
  {
    qDebug() << "CommandRetracement::runScript: invalid LEVEL_4, using default" << s;
    level4 = 0;
  }

  // LEVEL_5
  double level5 = 0;
  s = sg->value("LEVEL_5");
  if (vdi.toDouble(script, s, level5))
  {
    qDebug() << "CommandRetracement::runScript: invalid LEVEL_5, using default" << s;
    level5 = 0;
  }

  // LEVEL_6
  double level6 = 0;
  s = sg->value("LEVEL_6");
  if (vdi.toDouble(script, s, level6))
  {
    qDebug() << "CommandRetracement::runScript: invalid LEVEL_6, using default" << s;
    level6 = 0;
  }

  // CHART
  QString chart;
  s = sg->value("CHART");
  if (vdi.toString(script, s, chart))
  {
    qDebug() << "CommandRetracement::runScript: invalid CHART" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // Z
  int z = 0;
  s = sg->value("Z");
  if (vdi.toInteger(script, s, z))
  {
    qDebug() << "CommandRetracement::runScript: invalid Z, using default" << s;
    z = 0;
  }

  int id = script->nextROID();

  Data *co = new ChartObjectData;
  co->set(ChartObjectData::_COLOR, new DataColor(color));
  co->set(ChartObjectData::_DATE, new DataDateTime(date));
  co->set(ChartObjectData::_DATE_2, new DataDateTime(date2));
  co->set(ChartObjectData::_HIGH, new DataDouble(high));
  co->set(ChartObjectData::_LOW, new DataDouble(low));
  co->set(ChartObjectData::_CHART, new DataString(chart));
  co->set(ChartObjectData::_Z, new DataInteger(z));
  co->set(ChartObjectData::_ID, new DataInteger(id));
  co->set(ChartObjectData::_RO, new DataBool(TRUE));
  co->set(ChartObjectData::_TYPE, new DataString(QString("Retracement")));

  if (level1)
    co->set(ChartObjectData::_LEVEL_1, new DataDouble(level1));
  if (level2)
    co->set(ChartObjectData::_LEVEL_2, new DataDouble(level2));
  if (level3)
    co->set(ChartObjectData::_LEVEL_3, new DataDouble(level3));
  if (level4)
    co->set(ChartObjectData::_LEVEL_4, new DataDouble(level4));
  if (level5)
    co->set(ChartObjectData::_LEVEL_5, new DataDouble(level5));
  if (level6)
    co->set(ChartObjectData::_LEVEL_6, new DataDouble(level6));

  script->setData(QString::number(id), co);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
