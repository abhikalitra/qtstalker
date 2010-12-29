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

#include "PLOT.h"
#include "Globals.h"

#include <QtDebug>

PLOT::PLOT ()
{
  _type = _SERIAL;
  
  _method << "BACKGROUND_COLOR" << "FONT" << "GRID_COLOR" << "GRID" << "CROSSHAIRS_COLOR" << "CROSSHAIRS";
  _method << "NEW" << "DELETE" << "UPDATE";
}

int PLOT::command (Command *command)
{
  // PLOT,<METHOD>
  //   0     1 
  
  if (command->count() < 2)
  {
    qDebug() << "PLOT::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _BACKGROUND_COLOR:
      return backgroundColor(command);
      break;
    case _FONT:
      return font(command);
      break;
    case _GRID_COLOR:
      return gridColor(command);
      break;
    case _GRID:
      return grid(command);
      break;
    case _CROSSHAIRS_COLOR:
      return crosshairsColor(command);
      break;
    case _CROSSHAIRS:
      return crosshairs(command);
      break;
    case __NEW:
      return newPlot(command);
      break;
    case _DELETE:
      return deletePlot(command);
      break;
    case _UPDATE:
      return update(command);
      break;
    default:
      break;
  }

  return 0;
}

int PLOT::backgroundColor (Command *command)
{
  // PLOT,BACKGROUND_COLOR,<COLOR>
  //   0          1           2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::backgroundColor: invalid parm count" << command->count();
    return 1;
  }

  QColor c(command->parm(2));
  if (! c.isValid())
  {
    qDebug() << "PLOT::backgroundColor: invalid color" << command->parm(2);
    return 1;
  }

  g_middleMan->plotBackgroundColor(c);
  
  command->setReturnData("0");

  return 0;
}

int PLOT::font (Command *command)
{
  // PLOT,FONT,<FONT>
  //   0    1     2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::font: invalid parm count" << command->count();
    return 1;
  }

  QFont f;
  QStringList l = command->parm(2).split(";");
  f.fromString(l.join(","));

  g_middleMan->plotFont(f);

  command->setReturnData("0");

  return 0;
}

int PLOT::gridColor (Command *command)
{
  // PLOT,GRID_COLOR,<COLOR>
  //   0      1         2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::gridColor: invalid parm count" << command->count();
    return 1;
  }

  QColor c(command->parm(2));
  if (! c.isValid())
  {
    qDebug() << "PLOT::gridColor: invalid color" << command->parm(2);
    return 1;
  }

  g_middleMan->gridColor(c);

  command->setReturnData("0");

  return 0;
}

int PLOT::grid (Command *command)
{
  // PLOT,GRID,<STATUS>
  //   0    1     2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::grid: invalid parm count" << command->count();
    return 1;
  }

  bool ok;
  int status = command->parm(2).toInt(&ok);
  if (! ok)
  {
    qDebug() << "PLOT::grid: invalid status" << command->parm(2);
    return 1;
  }

  g_middleMan->grid(status);

  command->setReturnData("0");

  return 0;
}

int PLOT::crosshairsColor (Command *command)
{
  // PLOT,CROSSHAIRS_COLOR,<COLOR>
  //   0          1           2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::crosshairsColor: invalid parm count" << command->count();
    return 1;
  }

  QColor c(command->parm(2));
  if (! c.isValid())
  {
    qDebug() << "PLOT::crosshairsColor: invalid color" << command->parm(2);
    return 1;
  }

  g_middleMan->crosshairsColor(c);

  command->setReturnData("0");

  return 0;
}

int PLOT::crosshairs (Command *command)
{
  // PLOT,CROSSHAIRS,<STATUS>
  //   0      1         2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::crosshairs: invalid parm count" << command->count();
    return 1;
  }

  bool ok;
  int status = command->parm(2).toInt(&ok);
  if (! ok)
  {
    qDebug() << "PLOT::crosshairs: invalid status" << command->parm(2);
    return 1;
  }

  g_middleMan->crosshairs(status);

  command->setReturnData("0");

  return 0;
}

int PLOT::newPlot (Command *command)
{
  // PLOT,NEW,<NAME>
  //   0   1    2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::newPlot: invalid parm count" << command->count();
    return 1;
  }

  g_middleMan->indicatorNew(command->parm(2));

  command->setReturnData("0");

  return 0;
}

int PLOT::deletePlot (Command *command)
{
  // PLOT,DELETE,<NAME>,*
  //   0    1      2

  if (command->count() < 3)
  {
    qDebug() << "PLOT::deletePlot: invalid parm count" << command->count();
    return 1;
  }

  QStringList l;
  int pos = 2;
  for (; pos < command->count(); pos++)
    l << command->parm(pos);

  g_middleMan->indicatorDelete(l);

  command->setReturnData("0");

  return 0;
}

int PLOT::update (Command *command)
{
  // PLOT,UPDATE,<NAME>
  //   0     1     2

  if (command->count() != 3)
  {
    qDebug() << "PLOT::update: invalid parm count" << command->count();
    return 1;
  }

  g_middleMan->plotUpdate(command->parm(2));

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PLOT *o = new PLOT;
  return ((ScriptPlugin *) o);
}
