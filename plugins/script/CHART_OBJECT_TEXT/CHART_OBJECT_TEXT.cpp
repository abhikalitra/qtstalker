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

#include "CHART_OBJECT_TEXT.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>

CHART_OBJECT_TEXT::CHART_OBJECT_TEXT ()
{
  _plugin = "CHART_OBJECT_TEXT";
}

int CHART_OBJECT_TEXT::command (Command *command)
{
  // PARMS:
  // NAME
  // INDICATOR
  // EXCHANGE
  // SYMBOL
  // DATE
  // PRICE
  // COLOR
  // LABEL

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Setting co;
  co.setData("Type", QString("Text"));
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

  // verify price
  bool ok;
  command->parm("PRICE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PRICE" << command->parm("PRICE");
    return 1;
  }
  co.setData("Price", command->parm("PRICE"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  co.setData("Color", command->parm("COLOR"));

  co.setData("Text", command->parm("TEXT"));

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
  CHART_OBJECT_TEXT *o = new CHART_OBJECT_TEXT;
  return ((ScriptPlugin *) o);
}
