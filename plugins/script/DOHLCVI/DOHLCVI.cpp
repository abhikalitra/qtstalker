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

#include "DOHLCVI.h"
#include "Curve.h"
#include "Globals.h"
#include "InputType.h"

#include <QtDebug>

DOHLCVI::DOHLCVI ()
{
  _plugin = "DOHLCVI";
  _type = "INDICATOR";
  _keys << "NAME_DATE" << "NAME_OPEN" << "NAME_HIGH" << "NAME_LOW" << "NAME_CLOSE" << "NAME_VOLUME" << "NAME_OI";
}

int DOHLCVI::command (Command *command)
{
  // PARMS:
  // NAME_DATE
  // NAME_OPEN
  // NAME_HIGH
  // NAME_LOW
  // NAME_CLOSE
  // NAME_VOLUME
  // NAME_OI

  if (g_barData->count() < 1)
  {
    qDebug() << _plugin << "::command: no bars";
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  InputType it;
  QStringList fieldList = it.list();
  
  int loop = 0;
  for (; loop < _keys.count(); loop++)
  {
    QString name = command->parm(_keys.at(loop));
    if (! name.isEmpty())
    {
      Curve *line = i->line(name);
      if (! line)
      {
        line = it.input(g_barData, fieldList.at(loop));
        if (! line)
        {
          qDebug() << _plugin << "::command:" << _keys.at(loop) << "error";
          return 1;
        }
      
        line->setLabel(name);
        i->setLine(name, line);
      }
    }
  }
  
  command->setReturnCode("0");

  return 0;
}

void DOHLCVI::settings (Setting *set)
{
  set->clear();

  set->setData("KEYS", _keys.join(","));
  set->setData("PLUGIN", _plugin);
  set->setData("PLUGIN_TYPE", QString("INDICATOR"));
  set->setData("NAME_DATE", QString());
  set->setData("NAME_DATE:TYPE", QString("TEXT"));
  set->setData("NAME_OPEN", QString());
  set->setData("NAME_OPEN:TYPE", QString("TEXT"));
  set->setData("NAME_HIGH", QString());
  set->setData("NAME_HIGH:TYPE", QString("TEXT"));
  set->setData("NAME_LOW", QString());
  set->setData("NAME_LOW:TYPE", QString("TEXT"));
  set->setData("NAME_CLOSE", QString());
  set->setData("NAME_CLOSE:TYPE", QString("TEXT"));
  set->setData("NAME_VOLUME", QString());
  set->setData("NAME_VOLUME:TYPE", QString("TEXT"));
  set->setData("NAME_OI", QString());
  set->setData("NAME_OI:TYPE", QString("TEXT"));
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  DOHLCVI *o = new DOHLCVI;
  return ((Plugin *) o);
}
