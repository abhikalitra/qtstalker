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

#include "INDICATOR_PLOT_COLOR_COMPARE_INDEX.h"
#include "Operator.h"

#include <QtDebug>

INDICATOR_PLOT_COLOR_COMPARE_INDEX::INDICATOR_PLOT_COLOR_COMPARE_INDEX ()
{
}

int INDICATOR_PLOT_COLOR_COMPARE_INDEX::command (Command *command)
{
  // INDICATOR_PLOT_COLOR_COMPARE_INDEX,<NAME>,<OP>,<NAME2>,<NAME3>,<COLOR>
  //                   0                  1      2     3       4       5

  if (command->count() != 6)
  {
    qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: no indicator";
    return 1;
  }

  // verify NAME
  int pos = 1;
  int offset = 0;
  QString name = command->parm(pos);
  QStringList l = name.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name = l.at(0);
    
    bool ok;
    offset = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: invalid name" << command->parm(pos);
      return 1;
    }
  }
  
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: name not found" << name;
    return 1;
  }

  // verify OP
  pos++;
  Operator top;
  Operator::Type op = top.stringToOperator(command->parm(pos));
  if (op == -1)
  {
    qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: invalid operator" << command->parm(pos);
    return 1;
  }

  // verify NAME2
  pos++;
  int offset2 = 0;
  QString name2 = command->parm(pos);
  l = name2.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name2 = l.at(0);

    bool ok;
    offset2 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: invalid name2" << command->parm(pos);
      return 1;
    }
  }

  Curve *line2 = i->line(name2);
  if (! line2)
  {
    qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: name2 not found" << name2;
    return 1;
  }

  // verify NAME3
  pos++;
  int offset3 = 0;
  QString name3 = command->parm(pos);
  l = name3.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name3 = l.at(0);

    bool ok;
    offset3 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: invalid name3" << command->parm(pos);
      return 1;
    }
  }

  Curve *line3 = i->line(name3);
  if (! line3)
  {
    qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: name3 not found" << name3;
    return 1;
  }

  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_COLOR_COMPARE_INDEX::command: invalid color" << command->parm(pos);
    return 1;
  }

  // find lowest and highest index values
  int high = 0;
  int low = 0;
  line->keyRange(low, high);

  int tlow = 0;
  int thigh = 0;
  line2->keyRange(tlow, thigh);
  if (tlow < low)
    low = tlow;
  if (thigh > high)
    high = thigh;
  
  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = line->bar(loop - offset);
    if (! bar)
      continue;
    
    CurveBar *bar2 = line2->bar(loop - offset2);
    if (! bar2)
      continue;

    CurveBar *bar3 = line3->bar(loop - offset3);
    if (! bar3)
      continue;

    switch (op)
    {
      case Operator::_LessThan:
        if (bar->data() < bar2->data())
          bar3->setColor(color);
        break;
      case Operator::_LessThanEqual:
        if (bar->data() <= bar2->data())
          bar3->setColor(color);
        break;
      case Operator::_Equal:
        if (bar->data() == bar2->data())
          bar3->setColor(color);
        break;
      case Operator::_GreaterThanEqual:
        if (bar->data() >= bar2->data())
          bar3->setColor(color);
        break;
      case Operator::_GreaterThan:
        if (bar->data() > bar2->data())
          bar3->setColor(color);
        break;
      default:
        break;
    }
  }

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_PLOT_COLOR_COMPARE_INDEX *o = new INDICATOR_PLOT_COLOR_COMPARE_INDEX;
  return ((ScriptPlugin *) o);
}
