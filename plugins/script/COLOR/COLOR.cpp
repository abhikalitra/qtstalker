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

#include "COLOR.h"
#include "Globals.h"
#include "Operator.h"
#include "Strip.h"
#include "Curve.h"
#include "COLORWidget.h"

#include <QtDebug>

COLOR::COLOR ()
{
  _plugin = "COLOR";
  _type = "INDICATOR";
}

int COLOR::command (Command *command)
{
  // PARMS:
  // NAME
  // OP
  // NAME2
  // NAME3
  // COLOR

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
  int valueFlag2 = FALSE;
  double value2 = 0;
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
    value2 = name2.toDouble(&ok);
    if (! ok)
    {
      line2 = i->line(name2);
      if (! line2)
      {
        qDebug() << _plugin << "::command: NAME2 not found" << name2;
        return 1;
      }
    }

    valueFlag2++;
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

  if (! valueFlag2)
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
    if (valueFlag2)
      v2 = value2;
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

PluginWidget * COLOR::dialog (QWidget *p)
{
  return new COLORWidget(p);
}

void COLOR::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "NAME=Close";
  l << "OP=EQ";
  l << "NAME2=Close";
  l << "NAME3=Close";
  l << "COLOR=#FF0000";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  COLOR *o = new COLOR;
  return ((Plugin *) o);
}
