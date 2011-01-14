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

#include "PLOT_FONT.h"
#include "Globals.h"

#include <QtDebug>

PLOT_FONT::PLOT_FONT ()
{
}

int PLOT_FONT::command (Command *command)
{
  // PLOT_FONT,<FONT>
  //      0      1

  if (command->count() != 2)
  {
    qDebug() << "PLOT_FONT::command: invalid parm count" << command->count();
    return 1;
  }

  QFont f;
  QStringList l = command->parm(1).split(";");
  f.fromString(l.join(","));

  g_middleMan->plotFont(f);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PLOT_FONT *o = new PLOT_FONT;
  return ((ScriptPlugin *) o);
}
