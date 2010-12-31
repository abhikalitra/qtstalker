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

#include "MA.h"
#include "Curve.h"
#include "ta_libc.h"
#include "FunctionMA.h"

#include <QtDebug>

MA::MA ()
{
}

int MA::command (Command *command)
{
  // MA,<METHOD>,<NAME>,<INPUT>,<PERIOD>
  //  0    1        2      3       4

  if (command->count() != 5)
  {
    qDebug() << "MA::command: invalid settings count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "MA::command: no indicator";
    return 1;
  }

  int pos = 1;
  FunctionMA fma;
  int method = fma.typeFromString(command->parm(pos));
  if (method == -1)
  {
    qDebug() << "MA::command: invalid method" << command->parm(pos);
    return 1;
  }

  pos++;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "MA::command: duplicate name" << name;
    return 1;
  }

  pos++;
  Curve *in = i->line(command->parm(pos));
  if (! in)
  {
    qDebug() << "MA::command: input missing" << command->parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int period = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MA::command: invalid period" << command->parm(pos);
    return 1;
  }

  line = fma.calculate(in, period, method);
  if (! line)
    return 1;

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  MA *o = new MA;
  return ((ScriptPlugin *) o);
}
