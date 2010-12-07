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

#include "INDICATOR_PLOT_COLOR.h"
#include "Operator.h"

#include <QtDebug>

INDICATOR_PLOT_COLOR::INDICATOR_PLOT_COLOR ()
{
  _method << "SET" << "ALL" << "COMPARE_INDEX_ALL" << "COMPARE_VALUE_ALL";
}

int INDICATOR_PLOT_COLOR::command (Command *command)
{
  // INDICATOR_PLOT_COLOR,<METHOD>
  //            0            1

  if (command->count() < 2)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _SET:
      return setColor(command);
      break;
    case _ALL:
      return setColorAll(command);
      break;
    case _COMPARE_INDEX_ALL:
      return compareIndexAll(command);
      break;
    case _COMPARE_VALUE_ALL:
      return compareValueAll(command);
      break;
    default:
      break;
  }

  return 0;
}

int INDICATOR_PLOT_COLOR::setColor (Command *command)
{
  // INDICATOR_PLOT_COLOR,<METHOD>,<NAME>,<INDEX>,<COLOR>
  //           0             1       2       3       4

  if (command->count() != 5)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColor: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColor: no indicator";
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos);

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColor: name not found" << name;
    return 1;
  }

  pos++;
  bool ok;
  int index = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColor: invalid index value" << command->parm(pos);
    return 1;
  }

  CurveBar *bar = line->bar(index);
  if (! bar)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColor: bar not found" << command->parm(pos);
    return 1;
  }

  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColor: invalid color" << command->parm(pos);
    return 1;
  }

  bar->setColor(color);

  command->setReturnData("0");

  return 0;
}

int INDICATOR_PLOT_COLOR::setColorAll (Command *command)
{
  // INDICATOR_PLOT_COLOR,<METHOD>,<NAME>,<COLOR>
  //           0             1       2       3

  if (command->count() != 4)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColorAll: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColorAll: no indicator";
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos);

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColorAll: name not found" << name;
    return 1;
  }

  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_COLOR::setColorAll: invalid color" << command->parm(pos);
    return 1;
  }

  line->setAllColor(color);

  command->setReturnData("0");

  return 0;
}

/*
int INDICATOR_PLOT_COLOR::compareAll (Command &command)
{
  // INDICATOR_PLOT_COLOR,<METHOD>,<NAME>,<OP>,<OFFSET>,<COLOR>
  //           0             1       2      3     4        5

  // INDICATOR_PLOT_COLOR,<COMPARE>,<NAME>,<OP>,<NAME2>,<NAME3>,<COLOR>
  //           0             1        2      3     4       5       6

  if (command->count() != 6)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareAll: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareAll: no indicator";
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareAll: name not found" << name;
    return 1;
  }

  pos++;
  Operator top;
  Operator::Type op = top.stringToOperator(command->parm(pos));
  if (op == -1)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareAll: invalid operator" << command->parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int offset = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareAll: invalid offset value" << command->parm(pos);
    return 1;
  }

  if (offset < 0)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareAll: invalid offset value" << command->parm(pos);
    return 1;
  }

  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareAll: invalid color" << command->parm(pos);
    return 1;
  }

  QList<int> keys;
  line->keys(keys);

  int loop = offset;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = line->bar(keys.at(loop));
    CurveBar *bar2 = line->bar(keys.at(loop - offset));
    if (! bar2)
      continue;

    switch (op)
    {
      case Operator::_LessThan:
        if (bar->data() < bar2->data())
          bar->setColor(color);
        break;
      case Operator::_LessThanEqual:
        if (bar->data() <= bar2->data())
          bar->setColor(color);
        break;
      case Operator::_Equal:
        if (bar->data() == bar2->data())
          bar->setColor(color);
        break;
      case Operator::_GreaterThanEqual:
        if (bar->data() >= bar2->data())
          bar->setColor(color);
        break;
      case Operator::_GreaterThan:
        if (bar->data() > bar2->data())
          bar->setColor(color);
        break;
      default:
        break;
    }
  }

  command->setReturnData("0");

  emit signalDone();

  return 0;
}
*/

int INDICATOR_PLOT_COLOR::compareIndexAll (Command *command)
{
  // INDICATOR_PLOT_COLOR,<COMPARE>,<NAME>,<OP>,<NAME2>,<NAME3>,<COLOR>
  //           0             1        2      3     4       5       6

  if (command->count() != 7)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: no indicator";
    return 1;
  }

  // verify NAME
  int pos = 2;
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
      qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: invalid name" << command->parm(pos);
      return 1;
    }
  }
  
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: name not found" << name;
    return 1;
  }

  // verify OP
  pos++;
  Operator top;
  Operator::Type op = top.stringToOperator(command->parm(pos));
  if (op == -1)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: invalid operator" << command->parm(pos);
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
      qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: invalid name2" << command->parm(pos);
      return 1;
    }
  }

  Curve *line2 = i->line(name2);
  if (! line2)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: name2 not found" << name2;
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
      qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: invalid name3" << command->parm(pos);
      return 1;
    }
  }

  Curve *line3 = i->line(name3);
  if (! line3)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: name3 not found" << name3;
    return 1;
  }

  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareIndexAll: invalid color" << command->parm(pos);
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

int INDICATOR_PLOT_COLOR::compareValueAll (Command *command)
{
  // INDICATOR_PLOT_COLOR,<COMPARE>,<NAME>,<OP>,<VALUE>,<NAME2>,<COLOR>
  //           0             1        2      3     4       5       6

  if (command->count() != 7)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: no indicator";
    return 1;
  }

  // verify NAME
  int pos = 2;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: name not found" << name;
    return 1;
  }

  // verify OP
  pos++;
  Operator top;
  Operator::Type op = top.stringToOperator(command->parm(pos));
  if (op == -1)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: invalid operator" << command->parm(pos);
    return 1;
  }

  // verify VALUE
  pos++;
  bool ok;
  double value = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: invalid value" << command->parm(pos);
    return 1;
  }

  // verify NAME2
  pos++;
  int offset2 = 0;
  QString name2 = command->parm(pos);
  QStringList l = name2.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name2 = l.at(0);

    bool ok;
    offset2 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: invalid name2" << command->parm(pos);
      return 1;
    }
  }

  Curve *line2 = i->line(name2);
  if (! line2)
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: name2 not found" << name2;
    return 1;
  }

  pos++;
  QColor color(command->parm(pos));
  if (! color.isValid())
  {
    qDebug() << "INDICATOR_PLOT_COLOR::compareValueAll: invalid color" << command->parm(pos);
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

    CurveBar *bar2 = line2->bar(loop - offset2);
    if (! bar2)
      continue;

    switch (op)
    {
      case Operator::_LessThan:
        if (bar->data() < value)
          bar2->setColor(color);
        break;
      case Operator::_LessThanEqual:
        if (bar->data() <= value)
          bar2->setColor(color);
        break;
      case Operator::_Equal:
        if (bar->data() == value)
          bar2->setColor(color);
        break;
      case Operator::_GreaterThanEqual:
        if (bar->data() >= value)
          bar2->setColor(color);
        break;
      case Operator::_GreaterThan:
        if (bar->data() > value)
          bar2->setColor(color);
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
  INDICATOR_PLOT_COLOR *o = new INDICATOR_PLOT_COLOR;
  return ((ScriptPlugin *) o);
}
