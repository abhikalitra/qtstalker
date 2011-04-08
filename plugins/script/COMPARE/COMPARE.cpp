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

#include "COMPARE.h"
#include "Globals.h"
#include "Operator.h"
#include "Strip.h"
#include "Curve.h"
#include "InputType.h"
#include "RuleWidget.h"

#include <QtDebug>

COMPARE::COMPARE ()
{
  _plugin = "COMPARE";
  _type = "INDICATOR";
}

int COMPARE::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  QStringList l = settings->data("RULES").split(",", QString::SkipEmptyParts);
  int loop = 0;
  for (; loop < l.count(); loop += 5)
  {
    int pos = loop;
    
    // input 1
    int offset = 0;
    Curve *line = 0;
    QString name = l.at(pos++);
    QStringList tl = name.split(".", QString::SkipEmptyParts);
    if (tl.count() == 2)
    {
      line = i->line(tl.at(0));
      if (! line)
      {
        InputType it;
        line = it.input(bd, tl.at(0));
        if (! line)
        {
          qDebug() << _plugin << "::calculate: invalid INPUT 1" << name;
          return 1;
        }

        line->setLabel(tl.at(0));
        i->setLine(tl.at(0), line);
      }

      bool ok;
      offset = tl.at(1).toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::calculate: invalid INPUT 1" << name;
        return 1;
      }
    }
    else
    {
      line = i->line(name);
      if (! line)
      {
        InputType it;
        line = it.input(bd, name);
        if (! line)
        {
          qDebug() << _plugin << "::calculate: invalid INPUT 1" << name;
          return 1;
        }

        line->setLabel(name);
        i->setLine(name, line);
      }
    }

    // verify OP
    Operator top;
    Operator::Type op = top.stringToOperator(l.at(pos++));

    // input 2
    int valueFlag = FALSE;
    double value = 0;
    int offset2 = 0;
    Curve *line2 = 0;
    name = l.at(pos++);
    tl = name.split(".", QString::SkipEmptyParts);
    if (tl.count() == 2)
    {
      line2 = i->line(tl.at(0));
      if (! line2)
      {
        InputType it;
        line2 = it.input(bd, tl.at(0));
        if (! line2)
        {
          qDebug() << _plugin << "::calculate: invalid INPUT 2" << name;
          return 1;
        }

        line2->setLabel(tl.at(0));
        i->setLine(tl.at(0), line2);
      }

      bool ok;
      offset2 = tl.at(1).toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::calculate: invalid INPUT 2" << name;
        return 1;
      }
    }
    else
    {
      bool ok;
      value = name.toDouble(&ok);
      if (! ok)
      {
        line2 = i->line(name);
        if (! line2)
        {
          InputType it;
          line2 = it.input(bd, name);
          if (! line2)
          {
            qDebug() << _plugin << "::calculate: invalid INPUT 2" << name;
            return 1;
          }

          line->setLabel(name);
          i->setLine(name, line);
        }
      }

      valueFlag++;
    }

    // input 3
    int offset3 = 0;
    Curve *line3 = 0;
    name = l.at(pos++);
    tl = name.split(".", QString::SkipEmptyParts);
    if (tl.count() == 2)
    {
      line3 = i->line(tl.at(0));
      if (! line3)
      {
        InputType it;
        line3 = it.input(bd, tl.at(0));
        if (! line3)
        {
          qDebug() << _plugin << "::calculate: invalid INPUT 3" << name;
          return 1;
        }

        line3->setLabel(tl.at(0));
        i->setLine(tl.at(0), line3);
      }

      bool ok;
      offset3 = tl.at(1).toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::calculate: invalid INPUT 3" << name;
        return 1;
      }
    }
    else
    {
      line3 = i->line(name);
      if (! line3)
      {
        InputType it;
        line3 = it.input(bd, name);
        if (! line3)
        {
          qDebug() << _plugin << "::calculate: invalid INPUT 3" << name;
          return 1;
        }

        line3->setLabel(name);
        i->setLine(name, line3);
      }
    }

    // input 4
    int valueFlag2 = FALSE;
    double value2 = 0;
    int offset4 = 0;
    Curve *line4 = 0;
    name = l.at(pos++);
    tl = name.split(".", QString::SkipEmptyParts);
    if (tl.count() == 2)
    {
      line4 = i->line(tl.at(0));
      if (! line4)
      {
        InputType it;
        line4 = it.input(bd, tl.at(0));
        if (! line4)
        {
          qDebug() << _plugin << "::calculate: invalid INPUT 4" << name;
          return 1;
        }

        line4->setLabel(tl.at(0));
        i->setLine(tl.at(0), line4);
      }

      bool ok;
      offset4 = tl.at(1).toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::calculate: invalid INPUT 4" << name;
        return 1;
      }
    }
    else
    {
      bool ok;
      value2 = name.toDouble(&ok);
      if (! ok)
      {
        line4 = i->line(name);
        if (! line4)
        {
          InputType it;
          line4 = it.input(bd, name);
          if (! line4)
          {
            qDebug() << _plugin << "::calculate: invalid INPUT 4" << name;
            return 1;
          }

          line4->setLabel(name);
          i->setLine(name, line4);
        }
      }

      valueFlag2++;
    }

    // find lowest and highest index values
    // input 1
    int high = 0;
    int low = 0;
    line->keyRange(low, high);

    // input2
    if (! valueFlag)
    {
      int tlow = 0;
      int thigh = 0;
      line2->keyRange(tlow, thigh);
      if (tlow < low)
        low = tlow;
      if (thigh > high)
        high = thigh;
    }

    // input 3
    int tlow = 0;
    int thigh = 0;
    line3->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
    
    // input 4
    if (! valueFlag2)
    {
      int tlow = 0;
      int thigh = 0;
      line4->keyRange(tlow, thigh);
      if (tlow < low)
        low = tlow;
      if (thigh > high)
        high = thigh;
    }

    int loop2 = low;
    for (; loop2 <= high; loop2++)
    {
      CurveBar *bar = line->bar(loop2 - offset);
      if (! bar)
        continue;

      double v2 = 0;
      if (valueFlag)
        v2 = value;
      else
      {
        CurveBar *bar2 = line2->bar(loop2 - offset2);
        if (! bar2)
          continue;
        v2 = bar2->data();
      }

      CurveBar *bar3 = line3->bar(loop2 - offset3);
      if (! bar3)
        continue;

      double v4 = 0;
      if (valueFlag2)
        v4 = value;
      else
      {
        CurveBar *bar4 = line4->bar(loop2 - offset4);
        if (! bar4)
          continue;
        v4 = bar4->data();
      }

      if (top.test(bar->data(), op, v2))
        bar3->setData(v4);
    }
  }
  
  return 0;
}

int COMPARE::command (Command *command)
{
  // PARMS:
  // NAME
  // OP
  // NAME2
  // NAME3
  // COMPARE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify NAME
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
  }

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
    qDebug() << _plugin << "::command: invalid OP" << command->parm("OP");
    return 1;
  }

  // verify NAME2
  int offset2 = 0;
  int valueFlag = FALSE;
  double value = 0;
  Curve *line2 = 0;
  QString name2 = command->parm("NAME2");
  l = name2.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name2 = l.at(0);
    line2 = i->line(name2);
    if (! line2)
    {
      qDebug() << _plugin << "::command: NAME2 not found" << name2;
      return 1;
    }

    bool ok;
    offset2 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME2" << name2;
      return 1;
    }
  }
  else
  {
    bool ok;
    value = name2.toDouble(&ok);
    if (! ok)
    {
      line2 = i->line(name2);
      if (! line2)
      {
        qDebug() << _plugin << "::command: NAME2 not found" << name2;
        return 1;
      }
    }

    valueFlag++;
  }

  // verify NAME3
  int offset3 = 0;
  QString name3 = command->parm("NAME3");
  l = name3.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name3 = l.at(0);

    bool ok;
    offset3 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME3" << name3;
      return 1;
    }
  }

  Curve *line3 = i->line(name3);
  if (! line3)
  {
    qDebug() << _plugin << "::command: NAME3 not found" << name3;
    return 1;
  }

  QColor color(command->parm("COMPARE"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::command: invalid COMPARE" << command->parm("COMPARE");
    return 1;
  }

  // find lowest and highest index values
  int high = 0;
  int low = 0;
  line->keyRange(low, high);

  if (! valueFlag)
  {
    int tlow = 0;
    int thigh = 0;
    line2->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
  }

  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = line->bar(loop - offset);
    if (! bar)
      continue;

    double v2 = 0;
    if (valueFlag)
      v2 = value;
    else
    {
      CurveBar *bar2 = line2->bar(loop - offset2);
      if (! bar2)
        continue;
      v2 = bar2->data();
    }

    CurveBar *bar3 = line3->bar(loop - offset3);
    if (! bar3)
      continue;

    if (top.test(bar->data(), op, v2))
      bar3->setColor(color);
  }

  command->setReturnCode("0");

  return 0;
}

QWidget * COMPARE::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Input 1") << tr("Operator") << tr("Input 2") << tr("Output") << tr("Input 3");

  QList<int> format;
  format << RuleWidget::_INPUT << RuleWidget::_OPERATOR << RuleWidget::_INPUT;
  format << RuleWidget::_OUTPUT << RuleWidget::_INPUT;

  RuleWidget *w = new RuleWidget(p, _plugin);
  w->setRules(set, format, header);
  w->loadSettings();
  return w;
}

void COMPARE::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  COMPARE *o = new COMPARE;
  return ((Plugin *) o);
}
