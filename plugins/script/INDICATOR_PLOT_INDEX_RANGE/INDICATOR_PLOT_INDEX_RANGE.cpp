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

#include "INDICATOR_PLOT_INDEX_RANGE.h"

#include <QtDebug>

INDICATOR_PLOT_INDEX_RANGE::INDICATOR_PLOT_INDEX_RANGE ()
{
  _plugin = "INDICATOR_PLOT_INDEX_RANGE";
}

int INDICATOR_PLOT_INDEX_RANGE::command (Command *command)
{
  // PARMS:
  // NAME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::command: NAME not found" << name;
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);

  command->setReturnData(_plugin + "_START", QString::number(start));
  command->setReturnData(_plugin + "_END", QString::number(end));

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  INDICATOR_PLOT_INDEX_RANGE *o = new INDICATOR_PLOT_INDEX_RANGE;
  return ((Plugin *) o);
}
