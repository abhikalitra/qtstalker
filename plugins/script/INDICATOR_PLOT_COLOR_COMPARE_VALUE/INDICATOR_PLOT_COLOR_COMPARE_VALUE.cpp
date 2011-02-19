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

#include "INDICATOR_PLOT_COLOR_COMPARE_VALUE.h"
#include "Operator.h"

#include <QtDebug>

INDICATOR_PLOT_COLOR_COMPARE_VALUE::INDICATOR_PLOT_COLOR_COMPARE_VALUE ()
{
  _plugin = "INDICATOR_PLOT_COLOR_COMPARE_VALUE";
}

int INDICATOR_PLOT_COLOR_COMPARE_VALUE::command (Command *command)
{
  // PARMS:
  // NAME
  // OP
  // VALUE
  // NAME2
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify NAME
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::command: NAME not found" << name;
    return 1;
  }

  // verify OP
  Operator top;
  Operator::Type op = top.stringToOperator(command->parm("OP"));
  if (op == -1)
  {
    qDebug() << _plugin << "::command: invalid operator" << command->parm("OP");
    return 1;
  }

  // verify VALUE
  bool ok;
  double value = command->parm("VALUE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid VALUE" << command->parm("VALUE");
    return 1;
  }

  // verify NAME2
  QString name2 = command->parm("NAME2");
  Curve *line2 = i->line(name2);
  if (! line2)
  {
    qDebug() << _plugin << "::command: NAME2 not found" << name2;
    return 1;
  }

  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }

  // find lowest and highest index values
  int high = 0;
  int low = 0;
  line->keyRange(low, high);

  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = line->bar(loop);
    if (! bar)
      continue;

    CurveBar *bar2 = line2->bar(loop);
    if (! bar2)
      continue;

    if (top.test(bar->data(), op, value))
      bar2->setColor(color);
  }

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_PLOT_COLOR_COMPARE_VALUE *o = new INDICATOR_PLOT_COLOR_COMPARE_VALUE;
  return ((ScriptPlugin *) o);
}
