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
  int rows = settings->getInt("ROWS");
  int loop = 0;
  for (; loop < rows; loop++)
  {
    int col = 0;
    QString key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    QString name = settings->data(key);
    Curve *line = i->line(name);
    if (line)
    {
      qDebug() << _plugin << "::calculate: duplicate output" << name;
      return 1;
    }
    
    // verify OP
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    Operator top;
    Operator::Type op = top.stringToOperator(settings->data(key));

    // input 2
    int offset2 = 0;
    Curve *line2 = 0;
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    QString tname = settings->data(key);
    QStringList tl = tname.split(".", QString::SkipEmptyParts);
    if (tl.count() == 2)
    {
      line2 = i->line(tl.at(0));
      if (! line2)
      {
        InputType it;
        line2 = it.input(bd, tl.at(0));
        if (! line2)
        {
          qDebug() << _plugin << "::calculate: invalid NAME2" << tname;
          return 1;
        }

        line2->setZ(-1);
        line2->setLabel(tl.at(0));
        i->setLine(tl.at(0), line2);
      }

      bool ok;
      offset2 = tl.at(1).toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::calculate: invalid NAME2" << tname;
        return 1;
      }
    }
    else
    {
      line2 = i->line(tname);
      if (! line2)
      {
        InputType it;
        line2 = it.input(bd, tname);
        if (! line2)
        {
          qDebug() << _plugin << "::calculate: invalid NAME2" << tname;
          return 1;
        }

        line2->setZ(-1);
        line2->setLabel(tname);
        i->setLine(tname, line2);
      }
    }

    // input 3
    int valueFlag3 = FALSE;
    double value3 = 0;
    int offset3 = 0;
    Curve *line3 = 0;
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    tname = settings->data(key);
    tl = tname.split(".", QString::SkipEmptyParts);
    if (tl.count() == 2)
    {
      line3 = i->line(tl.at(0));
      if (! line3)
      {
        InputType it;
        line3 = it.input(bd, tl.at(0));
        if (! line3)
        {
          qDebug() << _plugin << "::calculate: invalid NAME3" << tname;
          return 1;
        }

        line3->setZ(-1);
        line3->setLabel(tl.at(0));
        i->setLine(tl.at(0), line3);
      }

      bool ok;
      offset3 = tl.at(1).toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::calculate: invalid NAME3" << tname;
        return 1;
      }
    }
    else
    {
      bool ok;
      value3 = tname.toDouble(&ok);
      if (! ok)
      {
        line3 = i->line(tname);
        if (! line3)
        {
          InputType it;
          line3 = it.input(bd, tname);
          if (! line3)
          {
            qDebug() << _plugin << "::calculate: invalid NAME3" << tname;
            return 1;
          }

          line3->setZ(-1);
          line3->setLabel(tname);
          i->setLine(tname, line3);
        }
      }

      valueFlag3++;
    }

    // input 4
    int valueFlag4 = FALSE;
    double value4 = 0;
    int offset4 = 0;
    Curve *line4 = 0;
    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    tname = settings->data(key);
    tl = tname.split(".", QString::SkipEmptyParts);
    if (tl.count() == 2)
    {
      line4 = i->line(tl.at(0));
      if (! line4)
      {
        InputType it;
        line4 = it.input(bd, tl.at(0));
        if (! line4)
        {
          qDebug() << _plugin << "::calculate: invalid NAME4" << tname;
          return 1;
        }

        line4->setZ(-1);
        line4->setLabel(tl.at(0));
        i->setLine(tl.at(0), line4);
      }

      bool ok;
      offset4 = tl.at(1).toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::calculate: invalid NAME4" << tname;
        return 1;
      }
    }
    else
    {
      bool ok;
      value4 = tname.toDouble(&ok);
      if (! ok)
      {
        line4 = i->line(tname);
        if (! line4)
        {
          InputType it;
          line4 = it.input(bd, tname);
          if (! line4)
          {
            qDebug() << _plugin << "::calculate: invalid NAME4" << tname;
            return 1;
          }

          line4->setZ(-1);
          line4->setLabel(tname);
          i->setLine(tname, line4);
        }
      }

      valueFlag4++;
    }

    // find lowest and highest index values
    // input 2
    int high = 0;
    int low = 0;
    line2->keyRange(low, high);

    // input3
    if (! valueFlag3)
    {
      int tlow = 0;
      int thigh = 0;
      line3->keyRange(tlow, thigh);
      if (tlow < low)
        low = tlow;
      if (thigh > high)
        high = thigh;
    }

    // input 4
    if (! valueFlag4)
    {
      int tlow = 0;
      int thigh = 0;
      line4->keyRange(tlow, thigh);
      if (tlow < low)
        low = tlow;
      if (thigh > high)
        high = thigh;
    }

    line = new Curve;
    int loop2 = low;
    for (; loop2 <= high; loop2++)
    {
      CurveBar *bar2 = line2->bar(loop2 - offset2);
      if (! bar2)
        continue;

      double v3 = 0;
      if (valueFlag3)
        v3 = value3;
      else
      {
        CurveBar *bar3 = line3->bar(loop2 - offset3);
        if (! bar3)
          continue;
        v3 = bar3->data();
      }

      double v4 = 0;
      if (valueFlag4)
        v4 = value4;
      else
      {
        CurveBar *bar4 = line4->bar(loop2 - offset4);
        if (! bar4)
          continue;
        v4 = bar4->data();
      }

      if (top.test(bar2->data(), op, v3))
        line->setBar(loop2, new CurveBar(v4));
    }

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    line->setAllColor(QColor(settings->data(key)));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    line->setType(settings->data(key));

    key = QString::number(loop) + "," + QString::number(col++) + ",DATA";
    line->setZ(settings->getInt(key));

    line->setLabel(name);
    i->setLine(name, line);
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
  // NAME4

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  // verify NAME2
  int offset2 = 0;
  QString tname = command->parm("NAME2");
  QStringList l = tname.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    tname = l.at(0);

    bool ok;
    offset2 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME2" << tname;
      return 1;
    }
  }
  Curve *line2 = i->line(tname);
  if (! line2)
  {
    qDebug() << _plugin << "::command: NAME2 not found" << tname;
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

  // verify NAME3
  int offset3 = 0;
  int valueFlag3 = FALSE;
  double value3 = 0;
  Curve *line3 = 0;
  tname = command->parm("NAME3");
  l = tname.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    tname = l.at(0);
    line3 = i->line(tname);
    if (! line3)
    {
      qDebug() << _plugin << "::command: NAME3 not found" << tname;
      return 1;
    }

    bool ok;
    offset3 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME3" << tname;
      return 1;
    }
  }
  else
  {
    bool ok;
    value3 = tname.toDouble(&ok);
    if (! ok)
    {
      line3 = i->line(tname);
      if (! line3)
      {
        qDebug() << _plugin << "::command: NAME3 not found" << tname;
        return 1;
      }
    }

    valueFlag3++;
  }

  // verify NAME4
  int offset4 = 0;
  int valueFlag4 = FALSE;
  double value4 = 0;
  Curve *line4 = 0;
  tname = command->parm("NAME4");
  l = tname.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    tname = l.at(0);
    line4 = i->line(tname);
    if (! line4)
    {
      qDebug() << _plugin << "::command: NAME4 not found" << tname;
      return 1;
    }

    bool ok;
    offset4 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME4" << tname;
      return 1;
    }
  }
  else
  {
    bool ok;
    value4 = tname.toDouble(&ok);
    if (! ok)
    {
      line4 = i->line(tname);
      if (! line4)
      {
        qDebug() << _plugin << "::command: NAME4 not found" << tname;
        return 1;
      }
    }

    valueFlag4++;
  }

  // find lowest and highest index values
  int high = 0;
  int low = 0;
  line2->keyRange(low, high);

  if (! valueFlag3)
  {
    int tlow = 0;
    int thigh = 0;
    line3->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
  }

  if (! valueFlag4)
  {
    int tlow = 0;
    int thigh = 0;
    line4->keyRange(tlow, thigh);
    if (tlow < low)
      low = tlow;
    if (thigh > high)
      high = thigh;
  }

  line = new Curve;
  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar2 = line2->bar(loop - offset2);
    if (! bar2)
      continue;

    double v3 = 0;
    if (valueFlag3)
      v3 = value3;
    else
    {
      CurveBar *bar3 = line3->bar(loop - offset3);
      if (! bar3)
        continue;
      v3 = bar3->data();
    }

    double v4 = 0;
    if (valueFlag4)
      v4 = value4;
    else
    {
      CurveBar *bar4 = line4->bar(loop - offset4);
      if (! bar4)
        continue;
      v4 = bar4->data();
    }

    if (top.test(bar2->data(), op, v3))
      line->setBar(loop, new CurveBar(v4));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

QWidget * COMPARE::dialog (QWidget *p, Setting *set)
{
  QStringList header;
  header << tr("Output") << tr("Input") << tr("Operator") << tr("Input 2") << tr("Input 3");
  header << tr("Color") << tr("Style") << tr("Plot");

  QList<int> format;
  format << RuleWidget::_OUTPUT << RuleWidget::_INPUT << RuleWidget::_OPERATOR;
  format << RuleWidget::_INPUT << RuleWidget::_INPUT << RuleWidget::_COLOR;
  format << RuleWidget::_STYLE << RuleWidget::_PLOT;

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
