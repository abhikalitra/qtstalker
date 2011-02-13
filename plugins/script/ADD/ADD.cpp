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

#include "ADD.h"
#include "Operator.h"

#include <QtDebug>

ADD::ADD ()
{
  _plugin = "ADD";
}

int ADD::command (Command *command)
{
  // PARMS:
  // NAME
  // NAME2
  // NAME3

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
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }
  line = new Curve;

  // verify NAME2
  int offset2 = 0;
  QString name2 = command->parm("NAME2");
  l = name2.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name2 = l.at(0);

    bool ok;
    offset2 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid NAME2" << name2;
      return 1;
    }
  }

  Curve *line2 = i->line(name2);
  if (! line2)
  {
    qDebug() << _plugin << "::command: NAME2 not found" << name2;
    return 1;
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

  // find lowest and highest index values
  int high = 0;
  int tlow = 0;
  int thigh = 0;
  line2->keyRange(tlow, thigh);
  if (thigh > high)
    high = thigh;
  
  line3->keyRange(tlow, thigh);
  if (thigh > high)
    high = thigh;
  
  int loop = 0;
  for (; loop <= high; loop++)
  {
    if (loop - offset < 0)
      continue;

    CurveBar *bar2 = line2->bar(loop - offset2);
    if (! bar2)
      continue;

    CurveBar *bar3 = line3->bar(loop - offset3);
    if (! bar3)
      continue;

    line->setBar(loop - offset, new CurveBar(bar2->data() + bar3->data()));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  ADD *o = new ADD;
  return ((ScriptPlugin *) o);
}
