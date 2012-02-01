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

#include "CommandChartUpdate.h"
#include "ScriptVerifyCurve.h"
#include "TypeEntity.h"
#include "KeyCurveData.h"
#include "KeyChartObject.h"
#include "Script.h"
#include "TypeThreadMessage.h"
#include "ThreadMessageFunctions.h"

#include <QtDebug>
#include <QObject>

CommandChartUpdate::CommandChartUpdate ()
{
  _name = "CHART_UPDATE";

  Data td;
  td.setLabel(QObject::tr("Chart Name"));
  Entity::set(QString("CHART"), td);

  td = Data(QString("date"));
  td.setLabel(QObject::tr("Date Data"));
  Entity::set(QString("DATE"), td);
}

QString CommandChartUpdate::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  Data chartName, dateName;
  Entity::toData(QString("CHART"), chartName);
  Entity::toData(QString("DATE"), dateName);

  // send chart date data
  Entity date;
  ScriptVerifyCurve svc;
  if (svc.curve(script, dateName.toString(), date))
  {
    qDebug() << "CommandChartUpdate::chartUpdate: date not found" << dateName.toString();
    return _returnCode;
  }
  date.set(QString("MESSAGE"), Data(TypeThreadMessage::_CHART_DATE));
  date.set(QString("CHART"), chartName);

  // send chart date
  ThreadMessageFunctions tmf;
  tmf.send(date, script);
  
  QList<QString> keys = script->dataKeys();

  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity dg;
    script->data(keys.at(loop), dg);

    switch ((TypeEntity::Key) dg.type())
    {
      case TypeEntity::_CURVE:
      {
        KeyCurveData keys;
        Data setting;
        if (dg.toData(keys.indexToString(KeyCurveData::_Z), setting))
          break;
        if (setting.toInteger() < 0)
          break;
        if (dg.toData(keys.indexToString(KeyCurveData::_CHART), setting))
          break;
        if (setting.toString() != chartName.toString())
          break;
        dg.set(QString("MESSAGE"), Data(TypeThreadMessage::_CHART_CURVE));
        dg.set(QString("CHART"), chartName);
        tmf.send(dg, script);
        break;
      }
      case TypeEntity::_CHART_OBJECT:
      {
        KeyChartObject keys;
        Data setting;
        if (dg.toData(keys.indexToString(KeyChartObject::_Z), setting))
          break;
        if (setting.toInteger() < 0)
          break;
        if (dg.toData(keys.indexToString(KeyChartObject::_CHART), setting))
          break;
        if (setting.toString() != chartName.toString())
          break;
        dg.set(QString("MESSAGE"), Data(TypeThreadMessage::_CHART_OBJECT));
        dg.set(QString("CHART"), chartName);
        tmf.send(dg, script);
        break;
      }
      default:
        break;
    }
  }

  // load chart objects
  Entity e;
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_CHART_LOAD_OBJECT));
  e.set(QString("CHART"), chartName);
  tmf.send(e, script);
  
  // update chart
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_CHART_UPDATE));
  tmf.send(e, script);

  _returnCode = "OK";
  return _returnCode;
}
