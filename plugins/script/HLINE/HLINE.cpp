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

#include "HLINE.h"
#include "ChartObject.h"
#include "HLineDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

HLINE::HLINE ()
{
  _plugin = "HLINE";
  _type = _INDICATOR;
}

int HLINE::calculate (BarData *, Indicator *i, Setting *settings)
{
  Setting co;
  QString key = "-" + QString::number(i->chartObjectCount() + 1);
  co.setData("TYPE", settings->data("TYPE"));
  co.setData("ID", key);
  co.setData("RO", 1);
  co.setData("PRICE", settings->data("PRICE"));
  co.setData("COLOR", settings->data("COLOR"));
  i->addChartObject(co);

  return 0;
}

int HLINE::command (Command *command)
{
  // PARMS:
  // TYPE
  // NAME
  // INDICATOR
  // EXCHANGE
  // SYMBOL
  // PRICE
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
  ChartObject tco;
  Setting *co = tco.settings();
  int saveFlag = 0;
  switch (typeList.indexOf(type))
  {
    case 0: // RO
    {
      QString key = "-" + QString::number(i->chartObjectCount() + 1);
      co->setData("TYPE", QString("HLine"));
      co->setData("ID", key);
      co->setData("RO", 1);
      break;
    }
    case 1:
      co->setData("TYPE", QString("HLine"));
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

  // verify price
  bool ok;
  command->parm("PRICE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PRICE" << command->parm("PRICE");
    return 1;
  }
  co->setData("PRICE", command->parm("PRICE"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  co->setData("COLOR", command->parm("COLOR"));

  if (saveFlag)
    tco.save();

  Setting set;
  co->copy(&set);
  i->addChartObject(set);

  command->setReturnCode("0");

  return 0;
}

QWidget * HLINE::dialog (QWidget *p, Setting *set)
{
  return new HLineDialog(p, set);
}

void HLINE::defaults (Setting *set)
{
  QSettings settings(g_globalSettings);
  set->setData("PLUGIN", _plugin);
  set->setData("COLOR", settings.value("default_hline_color", "white").toString());
  set->setData("TYPE", QString("HLine"));
  set->setData("PRICE", 0);
  set->setData("Z", 99);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  HLINE *o = new HLINE;
  return ((Plugin *) o);
}
