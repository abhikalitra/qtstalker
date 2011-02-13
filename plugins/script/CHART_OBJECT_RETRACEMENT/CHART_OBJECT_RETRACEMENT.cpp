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
  _plugin = "CHART_OBJECT_RETRACEMENT";
}

int CHART_OBJECT_RETRACEMENT::command (Command *command)
{
  // PARMS:
  // NAME
  // INDICATOR
  // EXCHANGE
  // SYMBOL
  // DATE
  // DATE2
  // COLOR
  // HIGH
  // LOW
  // LINE1>
  // LINE2>
  // LINE3>
  // LINE4>
  // LINE5>
  // LINE6>

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Setting co;
  co.setData("Type", QString("Retracement"));
  co.setData("ID", command->parm("NAME"));
  co.setData("Indicator", command->parm("INDICATOR"));
  co.setData("Exchange", command->parm("EXCHANGE"));
  co.setData("Symbol", command->parm("SYMBOL"));

  // verify date
  QDateTime dt = QDateTime::fromString(command->parm("DATE"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE" << command->parm("DATE");
    return 1;
  }
  co.setData("Date", command->parm("DATE"));

  // verify date2
  dt = QDateTime::fromString(command->parm("DATE2"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE2" << command->parm("DATE2");
    return 1;
  }
  co.setData("Date2", command->parm("DATE2"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  co.setData("Color", command->parm("COLOR"));

  // verify high
  bool ok;
  command->parm("HIGH").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid HIGH" << command->parm("HIGH");
    return 1;
  }
  co.setData("High", command->parm("HIGH"));

  // verify low
  command->parm("LOW").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LOW" << command->parm("LOW");
    return 1;
  }
  co.setData("Low", command->parm("LOW"));

  // verify all the possible retracement values
  int line = 1;
  for (; line < 7; line++)
  {
    QString k = "LINE" + QString::number(line);
    command->parm(k).toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid LINE" << k;
      continue;
    }

    QString key = "Line" + QString::number(line);
    co.setData(key, command->parm(k));
  }

  ChartObjectDataBase db;
  db.save(&co);

  i->addChartObject(co);

  command->setReturnCode("0");

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
