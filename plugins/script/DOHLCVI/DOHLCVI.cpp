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

#include "DOHLCVI.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

DOHLCVI::DOHLCVI ()
{
  _plugin = "DOHLCVI";
  _methods << "D" << "O" << "H" << "L" << "C" << "V" << "I";
}

int DOHLCVI::command (Command *command)
{
  // PARMS:
  // METHOD
  // NAME

  if (g_barData->count() < 1)
  {
    qDebug() << _plugin << "::command: no bars";
    return 1;
  }

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

  int method = _methods.indexOf(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
    return 1;
  }
  
  line = new Curve;
  int loop = 0;
  for (; loop < g_barData->count(); loop++)
  {
    Bar *b = g_barData->bar(loop);

    switch (method)
    {
      case 0: // date
      {
	CurveBar *cb = new CurveBar;
	cb->setDateTime(b->date());
        line->setBar(loop, cb);
	break;
      }
      case 1: // open
        line->setBar(loop, new CurveBar(b->open()));
	break;
      case 2: // high
        line->setBar(loop, new CurveBar(b->high()));
	break;
      case 3: // low
        line->setBar(loop, new CurveBar(b->low()));
	break;
      case 4: // close
        line->setBar(loop, new CurveBar(b->close()));
	break;
      case 5: // volume
        line->setBar(loop, new CurveBar(b->volume()));
	break;
      case 6: // oi
        line->setBar(loop, new CurveBar(b->oi()));
	break;
    }
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
  DOHLCVI *o = new DOHLCVI;
  return ((ScriptPlugin *) o);
}
