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
#include "ChartObject.h"

#include <QtDebug>

CHART_OBJECT_RETRACEMENT::CHART_OBJECT_RETRACEMENT ()
{
  _plugin = "CHART_OBJECT_RETRACEMENT";
}

int CHART_OBJECT_RETRACEMENT::command (Command *command)
{
  // PARMS:
  // TYPE
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

  // verify TYPE
  QString type = command->parm("TYPE");
  if (type.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid TYPE" << type;
    return 1;
  }

  QStringList typeList;
  typeList << "RO" << "RW";
  ChartObject tco;
  Setting *co = tco.settings();
  int saveFlag = 0;
  switch (typeList.indexOf(type))
  {
    case 0: // RO
    {
      QString key = "-" + QString::number(i->chartObjectCount() + 1);
      co->setData("TYPE", QString("Retracement"));
      co->setData("ID", key);
      co->setData("RO", 1);
      break;
    }
    case 1:
      co->setData("TYPE", QString("Retracement"));
      co->setData("ID", command->parm("NAME"));
      co->setData("INDICATOR", command->parm("INDICATOR"));
      co->setData("EXCHANGE", command->parm("EXCHANGE"));
      co->setData("SYMBOL", command->parm("SYMBOL"));
      saveFlag++;
      break;
    default:
      qDebug() << _plugin << "::command: invalid TYPE" << type;
      return 1;
      break;
  }

  // verify date
  QDateTime dt = QDateTime::fromString(command->parm("DATE"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE" << command->parm("DATE");
    return 1;
  }
  co->setData("DATE", command->parm("DATE"));

  // verify date2
  dt = QDateTime::fromString(command->parm("DATE2"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE2" << command->parm("DATE2");
    return 1;
  }
  co->setData("DATE2", command->parm("DATE2"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  co->setData("COLOR", command->parm("COLOR"));

  // verify high
  bool ok;
  command->parm("HIGH").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid HIGH" << command->parm("HIGH");
    return 1;
  }
  co->setData("HIGH", command->parm("HIGH"));

  // verify low
  command->parm("LOW").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid LOW" << command->parm("LOW");
    return 1;
  }
  co->setData("LOW", command->parm("LOW"));

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

    QString key = "LINE" + QString::number(line);
    co->setData(key, command->parm(k));
  }

  if (saveFlag)
    tco.save();

  Setting set;
  co->copy(&set);
  i->addChartObject(set);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CHART_OBJECT_RETRACEMENT *o = new CHART_OBJECT_RETRACEMENT;
  return ((Plugin *) o);
}
