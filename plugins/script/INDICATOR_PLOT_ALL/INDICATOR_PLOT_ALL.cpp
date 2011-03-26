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

#include "INDICATOR_PLOT_ALL.h"

#include <QtDebug>

INDICATOR_PLOT_ALL::INDICATOR_PLOT_ALL ()
{
  _plugin = "INDICATOR_PLOT_ALL";
}

int INDICATOR_PLOT_ALL::command (Command *command)
{
  // PARMS:
  // NAME
  // STYLE
  // COLOR
  // Z

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify name
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::command: NAME not found" << name;
    return 1;
  }

  // verify style
  int style = line->typeFromString(command->parm("STYLE"));
  if (style == -1)
  {
    qDebug() << _plugin << "::command: invalid STYLE" << command->parm("STYLE");
    return 1;
  }
  line->setType((Curve::Type) style);

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::setColorAll: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  line->setAllColor(color);

  // verify Z
  bool ok;
  int z = command->parm("Z").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid Z value" << command->parm("Z");
    return 1;
  }

  if (z < 0)
  {
    qDebug() << _plugin << "::command: invalid z value must be > -1" << z;
    return 1;
  }

  line->setZ(z);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  INDICATOR_PLOT_ALL *o = new INDICATOR_PLOT_ALL;
  return ((Plugin *) o);
}
