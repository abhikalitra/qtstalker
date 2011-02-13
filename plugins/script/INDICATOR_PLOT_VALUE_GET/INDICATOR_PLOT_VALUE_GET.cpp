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

#include "INDICATOR_PLOT_VALUE_GET.h"

#include <QtDebug>

INDICATOR_PLOT_VALUE_GET::INDICATOR_PLOT_VALUE_GET ()
{
  _plugin = "INDICATOR_PLOT_VALUE_GET";
}

int INDICATOR_PLOT_VALUE_GET::command (Command *command)
{
  // PARMS:
  // NAME
  // INDEX

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify name
  Curve *line = i->line(command->parm("NAME"));
  if (! line)
  {
    qDebug() << _plugin << "::command: name not found" << command->parm("NAME");
    return 1;
  }

  bool ok;
  int index = command->parm("INDEX").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid INDEX value" << command->parm("INDEX");
    return 1;
  }

  CurveBar *bar = line->bar(index);
  if (! bar)
  {
    qDebug() << _plugin << "::command: bar not found at index" << index;
    return 1;
  }

  QString s = QString::number(bar->data());
  command->setReturnData(_plugin + "_VALUE", s);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_PLOT_VALUE_GET *o = new INDICATOR_PLOT_VALUE_GET;
  return ((ScriptPlugin *) o);
}
