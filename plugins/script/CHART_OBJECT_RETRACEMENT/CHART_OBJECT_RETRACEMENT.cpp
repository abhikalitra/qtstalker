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

#include "CHART_OBJECT_RETRACEMENT.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>

CHART_OBJECT_RETRACEMENT::CHART_OBJECT_RETRACEMENT ()
{
  _method << "RO" << "RW";
}

int CHART_OBJECT_RETRACEMENT::command (Command *command)
{
  // CHART_OBJECT_RETRACEMENT,<METHOD>
  //          0           1

  if (command->count() < 2)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _RO:
      return createRO(command);
      break;
    case _RW:
      return createRW(command);
      break;
    default:
      break;
  }

  return 0;
}

int CHART_OBJECT_RETRACEMENT::createRW (Command *command)
{
  // CHART_OBJECT_RETRACEMENT,<METHOD>,<NAME>,<INDICATOR>,<EXCHANGE>,<SYMBOL>,<DATE>,<DATE2>,<COLOR>.<HIGH>,<LOW>,<LINE1>,*
  //             0               1        2         3         4         5       6       7       8      9      10    11    12*

  if (command->count() < 12)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRW: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRW: no indicator";
    return 1;
  }

  int pos = 2;
  Setting co;
  co.setData("Type", QString("Retracement"));
  co.setData("ID", command->parm(pos++));
  co.setData("Indicator", command->parm(pos++));
  co.setData("Exchange", command->parm(pos++));
  co.setData("Symbol", command->parm(pos++));

  // verify date
  QDateTime dt = QDateTime::fromString(command->parm(pos), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << "CHART_OBJECT_BUY::createRW: invalid date" << command->parm(pos);
    return 1;
  }
  co.setData("Date", command->parm(pos++));

  // verify date2
  dt = QDateTime::fromString(command->parm(pos), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << "CHART_OBJECT_BUY::createRW: invalid date2" << command->parm(pos);
    return 1;
  }
  co.setData("Date2", command->parm(pos++));

  // verify color
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRW: invalid color" << command->parm(pos);
    return 1;
  }
  co.setData("Color", command->parm(pos++));

  // verify high
  bool ok;
  command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRW: invalid high" << command->parm(pos);
    return 1;
  }
  co.setData("High", command->parm(pos++));

  // verify low
  command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRW: invalid low" << command->parm(pos);
    return 1;
  }
  co.setData("Low", command->parm(pos++));

  // verify all the possible retracement values
  int line = 1;
  for (; pos < command->count(); pos++, line++)
  {
    command->parm(pos).toDouble(&ok);
    if (! ok)
    {
      qDebug() << "CHART_OBJECT_RETRACEMENT::createRW: invalid line" << command->parm(pos);
      continue;
    }

    QString key = "Line" + QString::number(line);
    co.setData(key, command->parm(pos));
  }

  ChartObjectDataBase db;
  db.save(&co);

  i->addChartObject(co);

  command->setReturnData("0");

  return 0;
}

int CHART_OBJECT_RETRACEMENT::createRO (Command *command)
{
  // CHART_OBJECT_RETRACEMENT,<METHOD>,<DATE>,<DATE2>,<COLOR>.<HIGH>,<LOW>,<LINE1>,*
  //             0               1        2      3       4      5      6      7,   8*

  if (command->count() < 8)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: no indicator";
    return 1;
  }

  Setting co;
  QString key = "-" + QString::number(i->chartObjectCount() + 1);
  co.setData("Type", QString("Retracement"));
  co.setData("ID", key);
  co.setData("RO", 1);

  // verify date
  int pos = 2;
  QDateTime dt = QDateTime::fromString(command->parm(pos), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: invalid date" << command->parm(pos);
    return 1;
  }
  co.setData("Date", command->parm(pos++));

  // verify date2
  dt = QDateTime::fromString(command->parm(pos), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: invalid date2" << command->parm(pos);
    return 1;
  }
  co.setData("Date2", command->parm(pos++));

  // verify color
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: invalid color" << command->parm(pos);
    return 1;
  }
  co.setData("Color", command->parm(pos++));

  // verify high
  bool ok;
  command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: invalid high" << command->parm(pos);
    return 1;
  }
  co.setData("High", command->parm(pos++));

  // verify low
  command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: invalid low" << command->parm(pos);
    return 1;
  }
  co.setData("Low", command->parm(pos++));

  // verify all the possible retracement values
  int line = 1;
  for (; pos < command->count(); pos++, line++)
  {
    command->parm(pos).toDouble(&ok);
    if (! ok)
    {
      qDebug() << "CHART_OBJECT_RETRACEMENT::createRO: invalid line" << command->parm(pos);
      continue;
    }

    QString key = "Line" + QString::number(line);
    co.setData(key, command->parm(pos));
  }

  i->addChartObject(co);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_OBJECT_RETRACEMENT *o = new CHART_OBJECT_RETRACEMENT;
  return ((ScriptPlugin *) o);
}
