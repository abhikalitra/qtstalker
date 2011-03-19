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
#include "Strip.h"

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

  // verify NAME
  int offsetFlag = FALSE;
  int offset = 0;
  QString name = command->parm("NAME");
  QStringList l = name.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name = l.at(0);

    bool ok;
    offset = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME" << name;
      return 1;
    }

    offsetFlag = TRUE;
  }

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::command: NAME not found" << name;
    return 1;
  }

  // verify INDEX
  int index = 0;
  if (! offsetFlag)
  {
    bool ok;
    index = command->parm("INDEX").toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid INDEX value" << command->parm("INDEX");
      return 1;
    }
  }

  // get bar
  CurveBar *bar = 0;
  if (offsetFlag)
  {
    int high = 0;
    int low = 0;
    line->keyRange(low, high);
    
    bar = line->bar(high - offset);
    if (! bar)
    {
      qDebug() << _plugin << "::command: bar not found at offset" << high - offset;
      return 1;
    }
  }
  else
  {
    bar = line->bar(index);
    if (! bar)
    {
      qDebug() << _plugin << "::command: bar not found at index" << index;
      return 1;
    }
  }

  QString s;
  if (bar->isDate())
    s = bar->dateTime().toString(Qt::ISODate);
  else
  {
    Strip strip;
    strip.strip(bar->data(), 4, s);
  }
  
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
