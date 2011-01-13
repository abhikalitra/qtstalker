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

#include "INDICATOR_PLOT_INDEX.h"

#include <QtDebug>

INDICATOR_PLOT_INDEX::INDICATOR_PLOT_INDEX ()
{
  _method << "GET" << "SET" << "RANGE";
}

int INDICATOR_PLOT_INDEX::command (Command *command)
{
  // INDICATOR_PLOT_INDEX,<METHOD>
  //            0            1

  if (command->count() < 2)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _GET:
      return getIndex(command);
      break;
    case _SET:
      return setIndex(command);
      break;
    case _RANGE:
      return getRange(command);
      break;
    default:
      break;
  }

  return 0;
}

int INDICATOR_PLOT_INDEX::getIndex (Command *command)
{
  // INDICATOR_PLOT_INDEX,<METHOD>,<INDEX>
  //           0             1        2

  if (command->count() != 3)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getIndex: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getIndex: no indicator";
    return 1;
  }

  // verify index
  int pos = 2;
  QStringList l = command->parm(pos).split(".", QString::SkipEmptyParts);
  if (l.count() != 2)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getIndex: invalid index syntax" << command->parm(pos);
    return 1;
  }

  Curve *line = i->line(l.at(0));
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getIndex: name not found" << command->parm(pos);
    return 1;
  }

  bool ok;
  int index = l.at(1).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getIndex: invalid index value" << command->parm(pos);
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);
  CurveBar *bar = line->bar(end - index);
  if (! bar)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getIndex: bar not found" << command->parm(pos);
    return 1;
  }

  QString s = QString::number(bar->data());
  command->setReturnData(s);

  return 0;
}

int INDICATOR_PLOT_INDEX::setIndex (Command *command)
{
  // INDICATOR_PLOT_INDEX,<METHOD>,<NAME>,<INDEX>,<VALUE>,<COLOR>
  //           0             1       2       3       4       5

  if (command->count() != 6)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::setIndex: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::setIndex: no indicator";
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos);

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::setIndex: name not found" << name;
    return 1;
  }

  pos++;
  bool ok;
  int index = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::setIndex: invalid index value" << command->parm(pos);
    return 1;
  }

  pos++;
  double value = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::setIndex: invalid value" << command->parm(pos);
    return 1;
  }

  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_INDEX::setIndex: invalid color" << command->parm(pos);
    return 1;
  }

  line->setBar(index, new CurveBar(color, value));

  command->setReturnData("0");

  return 0;
}

int INDICATOR_PLOT_INDEX::getRange (Command *command)
{
  // INDICATOR_PLOT_INDEX,<METHOD>,<NAME>
  //           0             1       2

  if (command->count() != 3)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getRange: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getRange: no indicator";
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos);

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_INDEX::getRange: name not found" << name;
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);

  QStringList l;
  l << QString::number(start) << QString::number(end);
  command->setReturnData(l.join(","));

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_PLOT_INDEX *o = new INDICATOR_PLOT_INDEX;
  return ((ScriptPlugin *) o);
}
