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

#include "INDICATOR_PLOT_VALUE_SET.h"

#include <QtDebug>

INDICATOR_PLOT_VALUE_SET::INDICATOR_PLOT_VALUE_SET ()
{
  _plugin = "INDICATOR_PLOT_VALUE_SET";
}

int INDICATOR_PLOT_VALUE_SET::command (Command *command)
{
  // PARMS:
  // NAME
  // INDEX
  // VALUE
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::command: NAME not found" << name;
    return 1;
  }

  bool ok;
  int index = command->parm("INDEX").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid INDEX value" << command->parm("INDEX");
    return 1;
  }

  double value = command->parm("VALUE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid VALUE" << command->parm("VALUE");
    return 1;
  }

  QColor color(command->parm("COLOR"));
  if (! color.isValid())
    color = QColor(Qt::red);

  line->setBar(index, new CurveBar(color, value));

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  INDICATOR_PLOT_VALUE_SET *o = new INDICATOR_PLOT_VALUE_SET;
  return ((Plugin *) o);
}
