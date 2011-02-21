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

#include "INDICATOR_PLOT_DATE_GET.h"
#include "Globals.h"

#include <QtDebug>

INDICATOR_PLOT_DATE_GET::INDICATOR_PLOT_DATE_GET ()
{
  _plugin = "INDICATOR_PLOT_DATE_GET";
}

int INDICATOR_PLOT_DATE_GET::command (Command *command)
{
  // PARMS:
  // INDEX
  // DATE_FORMAT

  BarData *data = g_barData;
  if (! data)
  {
    qDebug() << _plugin << "::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  // verify INDEX
  bool ok;
  int index = command->parm("INDEX").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid INDEX value" << command->parm("INDEX");
    return 1;
  }

  Bar *bar = data->bar(index);
  if (! bar)
  {
    qDebug() << _plugin << "::command: bar not found at index" << index;
    return 1;
  }

  QString format = command->parm("DATE_FORMAT");
  if (format.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid DATE_FORMAT" << format;
    return 1;
  }

  command->setReturnData(_plugin + "_VALUE", bar->date().toString(format));

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_PLOT_DATE_GET *o = new INDICATOR_PLOT_DATE_GET;
  return ((ScriptPlugin *) o);
}
