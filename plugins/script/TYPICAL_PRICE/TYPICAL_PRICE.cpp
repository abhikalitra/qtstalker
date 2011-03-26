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

#include "TYPICAL_PRICE.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

TYPICAL_PRICE::TYPICAL_PRICE ()
{
  _plugin = "TYPICAL_PRICE";
}

int TYPICAL_PRICE::command (Command *command)
{
  // PARMS:
  // NAME
  // HIGH
  // LOW
  // CLOSE

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
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  Curve *high = i->line(command->parm("HIGH"));
  if (! high)
  {
    qDebug() << _plugin << "::command: invalid HIGH" << command->parm("HIGH");
    return 1;
  }

  Curve *low = i->line(command->parm("LOW"));
  if (! low)
  {
    qDebug() << _plugin << "::command: invalid LOW" << command->parm("LOW");
    return 1;
  }

  Curve *close = i->line(command->parm("CLOSE"));
  if (! close)
  {
    qDebug() << _plugin << "::command: invalid CLOSE" << command->parm("CLOSE");
    return 1;
  }

  int loop = 0;
  int end = 0;
  close->keyRange(loop, end);
  
  line = new Curve;
  for (; loop <= end; loop++)
  {
    CurveBar *h = high->bar(loop);
    if (! h)
      continue;

    CurveBar *l = low->bar(loop);
    if (! l)
      continue;

    CurveBar *c = close->bar(loop);
    if (! c)
      continue;
    
    double t = (h->data() + l->data() + c->data()) / 3.0;
    line->setBar(loop, new CurveBar(t));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  TYPICAL_PRICE *o = new TYPICAL_PRICE;
  return ((Plugin *) o);
}
