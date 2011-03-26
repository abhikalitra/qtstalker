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

#include "COPY_ARRAY.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

COPY_ARRAY::COPY_ARRAY ()
{
  _plugin = "COPY_ARRAY";
}

int COPY_ARRAY::command (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // VALUE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  // verify NAME
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  // verify INPUT
  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::command: INPUT not found" << command->parm("INPUT");
    return 1;
  }

  // verify VALUE
  int fillFlag = FALSE;
  double value = 0;
  QString s = command->parm("VALUE");
  if (! s.isEmpty())
  {
    bool ok;
    value = s.toDouble(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::command: invalid VALUE" << command->parm("VALUE");
      return 1;
    }

    fillFlag = TRUE;
  }

  QList<int> keys;
  in->keys(keys);

  int size = in->count();

  line = new Curve;
  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (fillFlag)
      line->setBar(keys.at(loop), new CurveBar(value));
    else      
      line->setBar(keys.at(loop), new CurveBar(bar->data()));
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
  COPY_ARRAY *o = new COPY_ARRAY;
  return ((Plugin *) o);
}
