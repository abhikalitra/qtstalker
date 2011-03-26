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

#include "CHART_OBJECT_TLINE.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>

CHART_OBJECT_TLINE::CHART_OBJECT_TLINE ()
{
  _plugin = "CHART_OBJECT_TLINE";
}

int CHART_OBJECT_TLINE::command (Command *command)
{
  // PARMS:
  // TYPE
  // NAME
  // INDICATOR
  // EXCHANGE
  // SYMBOL
  // DATE
  // DATE2
  // PRICE
  // PRICE2
  // COLOR

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
  Setting co;
  int saveFlag = 0;
  switch (typeList.indexOf(type))
  {
    case 0: // RO
    {
      QString key = "-" + QString::number(i->chartObjectCount() + 1);
      co.setData("Type", QString("TLine"));
      co.setData("ID", key);
      co.setData("RO", 1);
      break;
    }
    case 1:
      co.setData("Type", QString("TLine"));
      co.setData("ID", command->parm("NAME"));
      co.setData("Indicator", command->parm("INDICATOR"));
      co.setData("Exchange", command->parm("EXCHANGE"));
      co.setData("Symbol", command->parm("SYMBOL"));
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
  co.setData("Date", command->parm("DATE"));

  // verify date2
  dt = QDateTime::fromString(command->parm("DATE2"), Qt::ISODate);
  if (! dt.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE2" << command->parm("DATE2");
    return 1;
  }
  co.setData("Date2", command->parm("DATE2"));

  // verify price
  bool ok;
  command->parm("PRICE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PRICE" << command->parm("PRICE");
    return 1;
  }
  co.setData("Price", command->parm("PRICE"));

  // verify price2
  command->parm("PRICE2").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PRICE2" << command->parm("PRICE2");
    return 1;
  }
  co.setData("Price2", command->parm("PRICE2"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  co.setData("Color", command->parm("COLOR"));

  if (saveFlag)
  {
    ChartObjectDataBase db;
    db.save(&co);
  }

  i->addChartObject(co);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CHART_OBJECT_TLINE *o = new CHART_OBJECT_TLINE;
  return ((Plugin *) o);
}
