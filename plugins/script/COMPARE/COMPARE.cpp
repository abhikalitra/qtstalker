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
#include "Curve.h"
#include "COMPAREWidget.h"

#include <QtDebug>

COMPARE::COMPARE ()
{
  _plugin = "COMPARE";
  _type = "INDICATOR";
}

int COMPARE::command (Command *command)
{
  // PARMS:
  // NAME
  // OP
  // NAME2
  // NAME3
  // NAME4
  // NAME5

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

  // verify NAME5
  int offset5 = 0;
  int valueFlag5 = FALSE;
  double value5 = 0;
  Curve *line5 = 0;
  tname = command->parm("NAME5");
  l = tname.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    tname = l.at(0);
    line5 = i->line(tname);
    if (! line5)
    {
      qDebug() << _plugin << "::command: NAME5 not found" << tname;
      return 1;
    }

    bool ok;
    offset5 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME5" << tname;
      return 1;
    }
  }
  else
  {
    bool ok;
    value5 = tname.toDouble(&ok);
    if (! ok)
    {
      line5 = i->line(tname);
      if (! line5)
      {
        qDebug() << _plugin << "::command: NAME5 not found" << tname;
        return 1;
      }
    }

    valueFlag5++;
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

  if (! valueFlag5)
  {
    int tlow = 0;
    int thigh = 0;
    line5->keyRange(tlow, thigh);
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

    double v5 = 0;
    if (valueFlag5)
      v5 = value5;
    else
    {
      CurveBar *bar5 = line5->bar(loop - offset5);
      if (! bar5)
        continue;
      v5 = bar5->data();
    }
    
    if (top.test(bar2->data(), op, v3))
      line->setBar(loop, new CurveBar(v4));
    else
      line->setBar(loop, new CurveBar(v5));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

PluginWidget * COMPARE::dialog (QWidget *p)
{
  return new COMPAREWidget(p);
}

void COMPARE::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=" + _plugin;
  l << "OP=EQ";
  l << "NAME2=Close";
  l << "NAME3=Close";
  l << "NAME4=Close";
  l << "NAME5=Close";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  COMPARE *o = new COMPARE;
  return ((Plugin *) o);
}
