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

#include "CANDLES.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

CANDLES::CANDLES ()
{
  _plugin = "CANDLES";
}

int CANDLES::command (Command *command)
{
  // PARMS:
  // NAME
  // COLOR_UP
  // COLOR_DOWN
  // COLOR_NEUTRAL

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  int size = g_barData->count();
  if (size < 1)
  {
    qDebug() << _plugin << "::command: no bars";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  QColor upColor(Qt::green);
  QString s = command->parm("COLOR_UP");
  if (! s.isEmpty())
  {
    upColor.setNamedColor(s);
    if (! upColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_UP" << command->parm("COLOR_UP");
      return 1;
    }
  }

  QColor downColor(Qt::red);
  s = command->parm("COLOR_DOWN");
  if (! s.isEmpty())
  {
    downColor.setNamedColor(s);
    if (! downColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_DOWN" << command->parm("COLOR_DOWN");
      return 1;
    }
  }

  QColor neutralColor(Qt::blue);
  s = command->parm("COLOR_NEUTRAL");
  if (! s.isEmpty())
  {
    neutralColor.setNamedColor(s);
    if (! neutralColor.isValid())
    {
      qDebug() << _plugin << "::command: invalid COLOR_NEUTRAL" << command->parm("COLOR_NEUTRAL");
      return 1;
    }
  }

  line = new Curve(Curve::Candle);

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *b = g_barData->bar(loop);
    if (! b)
      continue;

    CurveBar *bar = new CurveBar;
    bar->setData(0, b->open());
    bar->setData(1, b->high());
    bar->setData(2, b->low());
    bar->setData(3, b->close());
    bar->setColor(neutralColor);

    Bar *yb = g_barData->bar(loop - 1);
    if (yb)
    {
      if (b->close() > yb->close())
        bar->setColor(upColor);
      else
      {
        if (b->close() < yb->close())
          bar->setColor(downColor);
      }
    }

    line->setBar(loop, bar);
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
  CANDLES *o = new CANDLES;
  return ((ScriptPlugin *) o);
}
